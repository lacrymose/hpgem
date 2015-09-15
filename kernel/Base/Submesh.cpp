/*
 This file forms part of hpGEM. This package has been developed over a number of years by various people at the University of Twente and a full list of contributors can be found at
 http://hpgem.org/about-the-code/team
 
 This code is distributed using BSD 3-Clause License. A copy of which can found below.
 
 
 Copyright (c) 2014, University of Twente
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "PhysicalElement.h"
#include "Submesh.h"
#include "Mesh.h"
#include "Element.h"
#include "Face.h"
#include "Edge.h"
#include "Geometry/PointPhysical.h" 
#include "FaceCacheData.h"
#include "ElementCacheData.h"

namespace Base
{
    void Submesh::add(Element* element)
    {
        logger.assert(element!=nullptr, "Invalid element passed");
        elements_.push_back(element);
    }
    
    void Submesh::addPush(Element* element, int processorID)
    {
        logger.assert(element!=nullptr, "Invalid element passed");
        pushElements_[processorID].push_back(element);
    }
    
    void Submesh::addPull(Element* element, int processorID)
    {
        logger.assert(element!=nullptr, "Invalid element passed");
        pullElements_[processorID].push_back(element);
    }
    
    void Submesh::add(Face* face)
    {
        logger.assert(face!=nullptr, "Invalid face passed");
        faces_.push_back(face);
    }
    
    void Submesh::add(Edge* edge)
    {
        logger.assert(edge!=nullptr, "Invalid edge passed");
        edges_.push_back(edge);
    }
    
    void Submesh::add(Node* node)
    {
        logger.assert(node!=nullptr, "Invalid node passed");
        nodes_.push_back(node);
    }
    
    void Submesh::clear()
    {
        elements_.clear();
        faces_.clear();
        edges_.clear();
        nodes_.clear();
        pullElements_.clear();
        pushElements_.clear();
        otherPulls_.clear();
    }

    void Submesh::processPullRequests()
    {
        //without MPI this routine promises to do nothing
#ifdef HPGEM_USE_MPI
        if(otherPulls_.size() > 0)
        {
            //reconstruct element->processor mapping
            MPIContainer& container = MPIContainer::Instance();
            std::size_t n = container.getNumProcessors();
            std::size_t rank = container.getProcessorID();
            std::vector<int> numberOfElements(n + 1, 0), cumulativeNumberOfElements(n + 1);
            auto& comm = container.getComm();
            numberOfElements[rank+1] = elements_.size()+1;
            comm.Allgather(MPI_IN_PLACE, 1, Base::Detail::toMPIType(*numberOfElements.data()), numberOfElements.data()+1, 1, Base::Detail::toMPIType(*numberOfElements.data()));
            std::partial_sum(numberOfElements.begin(), numberOfElements.end(), cumulativeNumberOfElements.begin());
            std::vector<std::size_t> elementIDs(cumulativeNumberOfElements[n]);
            auto IDIterator = elementIDs.begin() + cumulativeNumberOfElements[rank];
            for(Element* element: elements_)
            {
                *IDIterator = element->getID();
                IDIterator++;
            }
            //sentinel
            *IDIterator = std::numeric_limits<std::size_t>::max();
            std::sort(elementIDs.begin() + cumulativeNumberOfElements[rank], elementIDs.begin() + cumulativeNumberOfElements[rank+1]);
            comm.Allgatherv(MPI_IN_PLACE, 0, Base::Detail::toMPIType(n), elementIDs.data(), numberOfElements.data()+1 ,cumulativeNumberOfElements.data(), Base::Detail::toMPIType(n));

            //insert pull requests
            std::vector<std::vector<std::size_t>::iterator> searchIterators(n);
            //warning: this maps the pid where the data is to be pushed from to element id
            //(this is the processor the data is to be pushed to; usually this works the other way around)
            std::vector<std::vector<std::size_t> > newPushes(n);
            for(std::size_t i = 0; i < n; ++i)
            {
                searchIterators[i] = elementIDs.begin() + cumulativeNumberOfElements[i];
            }
            for(Element* element : otherPulls_)
            {
                for(std::size_t i = 0; i < n; ++i)
                {
                    if(i != rank)
                    {
                        while(*searchIterators[i] < element->getID())
                        {
                            searchIterators[i]++;
                        }
                        if(*searchIterators[i] == element->getID())
                        {
                            //this is one of the elements we are trying to pull
                            addPull(element, i);
                            newPushes[i].push_back(element->getID());
                        }
                    }
                }
            }

            //communicate again to process push requests
            std::vector<std::size_t> sendBuffer;
            std::vector<int> numberOfSendElements(n + 1, 0), numberOfRecieveElements(n + 1, 0);
            for(std::size_t i = 0; i < n; ++i)
            {
                numberOfSendElements[i + 1] = newPushes[i].size();
                for(std::size_t id : newPushes[i])
                {
                    sendBuffer.push_back(id);
                }
            }
            std::vector<int> cumulativeNumberOfSendElements(n + 1), cumulativeNumberOfRecieveElements(n + 1);
            std::partial_sum(numberOfSendElements.begin(), numberOfSendElements.end(), cumulativeNumberOfSendElements.begin());
            comm.Alltoall(numberOfSendElements.data()+1, 1, Base::Detail::toMPIType(*numberOfElements.data()),
                          numberOfRecieveElements.data()+1, 1, Base::Detail::toMPIType(*numberOfElements.data()));
            std::partial_sum(numberOfRecieveElements.begin(), numberOfRecieveElements.end(), cumulativeNumberOfRecieveElements.begin());
            std::vector<std::size_t> recieveBuffer(cumulativeNumberOfRecieveElements[n]);
            comm.Alltoallv(sendBuffer.data(), numberOfSendElements.data()+1, cumulativeNumberOfSendElements.data(), Base::Detail::toMPIType(n),
                           recieveBuffer.data(), numberOfRecieveElements.data()+1, cumulativeNumberOfRecieveElements.data(), Base::Detail::toMPIType(n));
            auto pushIterator = recieveBuffer.begin();
            for(std::size_t i = 0; i < n; ++i)
            {
                for(;pushIterator!=recieveBuffer.begin() + cumulativeNumberOfRecieveElements[i+1];++pushIterator)
                {
                    auto candidate = std::find_if(elements_.begin(), elements_.end(), [&](Element* a){return a->getID() == *pushIterator;});
                    logger.assert(candidate != elements_.end(), "element with ID % does not belong here", *pushIterator);
                    addPush(*candidate, i);
                }
            }
            otherPulls_.clear();
        }
#endif
    }
}













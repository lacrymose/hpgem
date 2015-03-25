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
#ifdef HPGEM_USE_MPI
#include <mpi.h>
#endif

#include "MpiContainer.h"
#include "Mesh.h"
#include "Element.h"
#include "Face.h"
#include "Edge.h"
#include "ElementFactory.h"
#include "FaceFactory.h"
#include "Geometry/PointPhysical.h"
#include "Geometry/ReferenceGeometry.h"
#include "Geometry/PointReference.h"
#include "FaceCacheData.h"
#include "ElementCacheData.h"

#ifdef HPGEM_USE_METIS
#include <metis.h>
#endif

namespace Base
{
    
    Mesh::Mesh()
            : hasToSplit_(false), localProcessorID_(0), elementcounter_(0), faceCounter_(0), edgeCounter_(0), nodeCounter_(0)
    {
    }
    
    /// IFCD: this copy constructor is work in progress
    Mesh::Mesh(const Mesh& orig)
            : hasToSplit_(orig.hasToSplit_), localProcessorID_(orig.localProcessorID_),
        elementcounter_(orig.elementcounter_), faceCounter_(orig.faceCounter_), edgeCounter_(orig.edgeCounter_), nodeCounter_(orig.nodeCounter_), 
        points_(orig.points_)
    {
        //Make Elements
        
    }
    
    Mesh::~Mesh()
    {
        clear();        
    }
    
    Element* Mesh::addElement(const std::vector<std::size_t>& globalNodeIndexes)
    {
        elements_.push_back(ElementFactory::instance().makeElement(globalNodeIndexes, points_, elementcounter_));
        ++elementcounter_;
        hasToSplit_ = true;
        return elements_.back();
    }
    
    bool Mesh::addFace(Element* leftElementPtr, std::size_t leftElementLocalFaceNo, Element* rightElementPtr, std::size_t rightElementLocalFaceNo, const Geometry::FaceType& faceType)
    {
        logger.assert(leftElementPtr!=nullptr, "Invalid element passed");
        if (rightElementPtr == nullptr)
        {
            faces_.push_back(FaceFactory::instance().makeFace(leftElementPtr, leftElementLocalFaceNo, faceType, faceCounter_));
        }
        else
        {
            faces_.push_back(FaceFactory::instance().makeFace(leftElementPtr, leftElementLocalFaceNo, rightElementPtr, rightElementLocalFaceNo, faceCounter_));
        }
        ++faceCounter_;
        hasToSplit_ = true;
        return true;
    }
    
    void Mesh::addEdge()
    {
        edges_.push_back(new Edge(edgeCounter_));
        ++edgeCounter_;
        hasToSplit_ = true;
    }
    
    void Mesh::addNode(Geometry::PointPhysical node)
    {
        points_.push_back(node);
        //don't distribute the points here, it will confuse the elements
    }
    
    void Mesh::addVertex()
    {
        nodes_.push_back(new Node(nodeCounter_));
        ++nodeCounter_;
        hasToSplit_ = true;
    }
    
    void Mesh::split()
    {
        std::vector<int> partition(elements_.size()); //output
        //split the mesh
        int pid = 0;
#ifdef HPGEM_USE_MPI
#ifdef HPGEM_USE_METIS
        pid = MPIContainer::Instance().getProcessorID();
        int nProcs = MPIContainer::Instance().getNumProcessors();

        if (pid == 0 && nProcs > 1)
        {   
            logger(INFO, "start of metis");

            int one = 1; //actually the number of constraints. This can be increased for example when we want to distribute an entire mesh tree in one go (while keeping each of the levels balanced) - increasing this number turns imbalance into a vector
            int numberOfElements = elements_.size();

            //int mpiCommSize=4;
            float imbalance = 1.001;//explicitly put the default for later manipulation
            int totalCutSize;//output
            
            std::vector<int> xadj(numberOfElements + 1);//for some reason c-style arrays break somewhere near 1e6 faces in a mesh, so use vectors
            std::vector<int> adjncy(2 * faces_.size());//if this basic connectivity structure turns out to be very slow for conforming meshes, some improvements can be made
            int connectionsUsed(0), xadjCounter(0);
            for (Element* element : elements_)
            {   
                xadj[xadjCounter] = connectionsUsed;
                xadjCounter++;
                for (int i = 0; i < element->getReferenceGeometry()->getNrOfCodim1Entities(); ++i)
                {   
                    const Face* face = element->getFace(i);
                    if (face->isInternal())
                    {   
                        if (element == face->getPtrElementLeft())
                        {   
                            adjncy[connectionsUsed] = face->getPtrElementRight()->getID();
                            connectionsUsed++;
                        }
                        else
                        {   
                            adjncy[connectionsUsed] = face->getPtrElementLeft()->getID();
                            connectionsUsed++;
                        }
                    } //boundary faces don't generate connections
                }
            }
            xadj[xadjCounter] = connectionsUsed;

            int metisOptions[METIS_NOPTIONS];
            METIS_SetDefaultOptions(metisOptions);

            metisOptions[METIS_OPTION_CTYPE] = METIS_CTYPE_SHEM;
            metisOptions[METIS_OPTION_RTYPE] = METIS_RTYPE_FM;

            //the empty arguments provide options for fine-tuning the weights of nodes, edges and processors, these are currently assumed to be the same
            METIS_PartGraphKway(&numberOfElements, &one, &xadj[0], &adjncy[0], NULL, NULL, NULL, &nProcs, NULL, &imbalance, metisOptions, &totalCutSize, &partition[0]);
            //mpiCommunicator.Bcast((void *)&partition[0],partition.size(),MPI::INT,0);//broadcast the computed partition to all the nodes
            logger(INFO, "Done splitting mesh.");

        }

        MPIContainer::Instance().broadcast(partition, 0);

#endif
#endif
        submeshes_.clear();
        auto elementIterator = elements_.begin();
        for (auto targetIterator = partition.begin(); targetIterator != partition.end(); ++targetIterator, ++elementIterator)
        {
            
            if (pid == *targetIterator)
            {
                submeshes_.add(*elementIterator);
            }
        }
        for (Base::Face* face : faces_)
        {
            //Are we part of this face?
            if (partition[face->getPtrElementLeft()->getID()] == pid || (face->isInternal() && partition[face->getPtrElementRight()->getID()] == pid))
            {
                //if we are a part of this face
                submeshes_.add(face);
                
                if (face->isInternal() && (partition[face->getPtrElementLeft()->getID()] != partition[face->getPtrElementRight()->getID()]))
                {
                    face->setFaceType(Geometry::FaceType::SUBDOMAIN_BOUNDARY);
                    if (partition[face->getPtrElementLeft()->getID()] == pid)
                    {
                        //don't send to yourself, ask the element on the other side what pid to sent to
                        submeshes_.addPush(face->getPtrElementLeft(), partition[face->getPtrElementRight()->getID()]);
                        //if you receive, the source is the owner of the element
                        submeshes_.addPull(face->getPtrElementRight(), partition[face->getPtrElementRight()->getID()]);
                    }
                    else
                    {
                        submeshes_.addPush(face->getPtrElementRight(), partition[face->getPtrElementLeft()->getID()]);
                        submeshes_.addPull(face->getPtrElementLeft(), partition[face->getPtrElementLeft()->getID()]);
                    }
                }
            }
        }
        
        //edges and nodes are only used to construct conforming basis-functions (i.e. GlobalAssembly))
        for (Base::Edge* edge : edges_)
        {
            if (partition[edge->getElement(0)->getID()] == pid)
            {
                submeshes_.add(edge);
            }
        }
        for (Base::Node* node : nodes_)
        {
            if (partition[node->getElement(0)->getID()] == pid)
            {
                submeshes_.add(node);
            }
        }
        hasToSplit_ = false;
    }
    
    void Mesh::clear()
    {
        for (Element* element : elements_)
        {
            delete element;
        }
        for (Face* face : faces_)
        {
            delete face;
        }
        for (Edge* edge : edges_)
        {
            delete edge;
        }
        for (Node* node : nodes_)
        {
            delete node;
        }
        elements_.clear();
        faces_.clear();
        edges_.clear();
        nodes_.clear();
        submeshes_.clear();
        elementcounter_ = 0;
        faceCounter_ = 0;
        edgeCounter_ = 0;
        nodeCounter_ = 0;
        points_.clear();
    }
    
    const std::vector<Element*>& Mesh::getElementsList(IteratorType part) const
    {
        if (part == IteratorType::LOCAL)
        {
            logger.assert_always(!hasToSplit_, "Please call getElementsList() on a modifiable mesh at least once"
                    "\nbefore calling getElementsList() const");
            return submeshes_.getElementsList();
        }
        else
        {
            return elements_;
        }
    }
    
    std::vector<Element*>& Mesh::getElementsList(IteratorType part)
    {
        if (part == IteratorType::LOCAL)
        {
            if (hasToSplit_)
            {
                split();
            }
            return submeshes_.getElementsList();
        }
        else
        {
            return elements_;
        }
    }
    
    const std::vector<Face*>& Mesh::getFacesList(IteratorType part) const
    {
        if (part == IteratorType::LOCAL)
        {
            logger.assert_always(!hasToSplit_, "Please call getFacesList() on a modifiable mesh at least once"
                    "\nbefore calling getFacesList() const");
            return submeshes_.getFacesList();
        }
        else
        {
            return faces_;
        }
    }
    
    std::vector<Face*>& Mesh::getFacesList(IteratorType part)
    {
        if (part == IteratorType::LOCAL)
        {
            if (hasToSplit_)
            {
                split();
            }
            return submeshes_.getFacesList();
        }
        else
        {
            return faces_;
        }
    }
    
    const std::vector<Edge*>& Mesh::getEdgesList(IteratorType part) const
    {
        if (part == IteratorType::LOCAL)
        {
            logger.assert_always(!hasToSplit_, "Please call getEdgesList() on a modifiable mesh at least once"
                    "\nbefore calling getEdgesList() const");
            return submeshes_.getEdgesList();
        }
        else
        {
            return edges_;
        }
    }
    
    std::vector<Edge*>& Mesh::getEdgesList(IteratorType part)
    {
        if (part == IteratorType::LOCAL)
        {
            if (hasToSplit_)
            {
                split();
            }
            return submeshes_.getEdgesList();
        }
        else
        {
            return edges_;
        }
    }
    
    const std::vector<Node*>& Mesh::getVerticesList(IteratorType part) const
    {
        if (part == IteratorType::LOCAL)
        {
            logger.assert_always(!hasToSplit_, "Please call getVerticesListList() on a modifiable mesh at least once"
                    "\nbefore calling getVerticesList() const");
            return submeshes_.getNodesList();
        }
        else
        {
            return nodes_;
        }
    }
    
    std::vector<Node*>& Mesh::getVerticesList(IteratorType part)
    {
        if (part == IteratorType::LOCAL)
        {
            if (hasToSplit_)
            {
                split();
            }
            return submeshes_.getNodesList();
        }
        else
        {
            return nodes_;
        }
    }
    
    const std::vector<Geometry::PointPhysical>& Mesh::getNodes() const
    {
        //for historic reasons points_ is referenced directly during element 
        //creation and therefore cannot be distributed
        return points_;
    }
    
    std::vector<Geometry::PointPhysical>& Mesh::getNodes()
    {
        //for historic reasons points_ is referenced directly during element 
        //creation and therefore cannot be distributed
        return points_;
    }

}
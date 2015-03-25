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

#ifndef MAPPINGCODIMENSIONS_HPP_
#define MAPPINGCODIMENSIONS_HPP_

#include <vector>
#include "Logger.h"

namespace Geometry
{
    class ReferenceGeometry;
    
    class MappingReferenceToReference;
    
    /*!
     *  ~OC~ CodimMaps can be used as base class for ReferenceGeometry
     *  implementations. It contains the methods necessary to organize the
     *  topological and geometrical information regarding lower-dimensional
     *  components, as well as the mappings of the reference geometry onto
     *  itself. The former items concern mappings of faces and edges onto the
     *  element, the latter is needed for the face-to-face mappings. The class
     *  is specialized for each dimension, mainly because a "recursive"
     *  derivation also becomes very hard to manage (relics of that may still
     *  be found in old svn versions).  
     */

    class MappingCodimensions
    {
    public:
        
        virtual std::size_t
        getCodim0MappingIndex(const std::vector<std::size_t>&, const std::vector<std::size_t>&) const = 0;

        virtual const MappingReferenceToReference* getCodim0MappingPtr(const std::size_t) const = 0;

        const MappingReferenceToReference* getCodim0MappingPtr(const std::vector<std::size_t>& n1, const std::vector<std::size_t>& n2) const
        {
            return getCodim0MappingPtr(getCodim0MappingIndex(n1, n2));
        }
        
        virtual std::size_t getNrOfCodim1Entities() const
        {
            return 0;
        }
        
        virtual std::vector<std::size_t> getCodim1EntityLocalIndices(const std::size_t) const
        {
            std::vector<std::size_t> dummy(1);
            logger(ERROR, "The dimension of given entity is too low to warrant maps of this codimension.\n");
            return dummy;
        
        }
        
        virtual const MappingReferenceToReference* getCodim1MappingPtr(const std::size_t) const
        {
            logger(ERROR, "The dimension of given entity is too low to warrant maps of this codimension.\n");
            return 0;
        }
        
        virtual const ReferenceGeometry* getCodim1ReferenceGeometry(const std::size_t) const
        {
            logger(ERROR, "The dimension of given entity is too low to warrant maps of this codimension.\n");
            return 0;
        }
        
        virtual std::size_t getNrOfCodim2Entities() const
        {
            return 0;
        }
        
        virtual std::vector<std::size_t> getCodim2EntityLocalIndices(const std::size_t) const
        {
            std::vector<std::size_t> dummy(1);
            logger(ERROR, "The dimension of given entity is too low to warrant maps of this codimension.\n");
            return dummy;
        }
        
        virtual const MappingReferenceToReference* getCodim2MappingPtr(const std::size_t) const
        {
            logger(ERROR, "The dimension of given entity is too low to warrant maps of this codimension.\n");
            return 0;
        }
        
        virtual const ReferenceGeometry* getCodim2ReferenceGeometry(const std::size_t) const
        {
            logger(ERROR, "The dimension of given entity is too low to warrant maps of this codimension.\n");
            return 0;
        }
        
        virtual std::size_t getNrOfCodim3Entities() const
        {
            return 0;
        }
        
        virtual std::vector<std::size_t> getCodim3EntityLocalIndices(const std::size_t) const
        {
            std::vector<std::size_t> dummy(1);
            logger(ERROR, "The dimension of given entity is too low to warrant maps of this codimension.\n");
            return dummy;
        }
        
        virtual ~MappingCodimensions()
        {
        }
    };
}
#endif
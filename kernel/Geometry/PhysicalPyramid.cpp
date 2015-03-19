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
#include "PhysicalPyramid.h"

#include "ReferencePyramid.h"
#include "PointPhysical.h"
#include "PointReference.h"

namespace Geometry
{
    PhysicalPyramid::PhysicalPyramid(const std::vector<std::size_t>& globalNodeIndexes, const std::vector<PointPhysical>& nodes)
            : PhysicalGeometry(globalNodeIndexes, nodes, &ReferencePyramid::Instance())
    {
    }
    
    std::vector<std::size_t> PhysicalPyramid::getGlobalFaceNodeIndices(const std::size_t face) const
    {
        logger.assert(face < getNrOfFaces(), "Asked for face %, but there are only % faces in a %", face, getNrOfFaces(), getRefGeometry()->getName());
        std::vector<std::size_t> indexes(4);
        if (face == 0)
        {
            for (std::size_t i = 0; i < 4; ++i)
            {
                indexes[i] = globalNodeIndexes_[refGeometry_->getLocalNodeIndexFromFaceAndIndexOnFace(face, i)];
            }
        }
        else
        {
            for (std::size_t i = 0; i < 3; ++i)
            {
                indexes[i] = globalNodeIndexes_[refGeometry_->getLocalNodeIndexFromFaceAndIndexOnFace(face, i)];
            }
        }
        return indexes;
    }
    
    std::vector<std::size_t> PhysicalPyramid::getLocalFaceNodeIndices(const std::size_t face) const
    {
        logger.assert(face < getNrOfFaces(), "Asked for face %, but there are only % faces in a %", face, getNrOfFaces(), getRefGeometry()->getName());
        std::vector<std::size_t> indexes(4);
        if (face == 0)
        {
            for (std::size_t i = 0; i < 4; ++i)
            {
                indexes[i] = refGeometry_->getLocalNodeIndexFromFaceAndIndexOnFace(face, i);
            }
        }
        else
        {
            for (std::size_t i = 0; i < 3; ++i)
            {
                indexes[i] = refGeometry_->getLocalNodeIndexFromFaceAndIndexOnFace(face, i);
            }
        }
        return indexes;
    }
}

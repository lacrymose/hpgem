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
#include "ReferenceHypercube.h"
#include "ReferenceCube.h"
#include "ReferenceSquare.h"
#include "ReferenceLine.h"
#include "Geometry/PointReference.h"
#include "Mappings/MappingToRefCubeToHypercube.h"

namespace Geometry
{
    std::size_t ReferenceHypercube::localNodeIndexes_[8][8] = { {0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 8, 9, 10, 11}, {0, 1, 4, 5, 8, 9, 12, 13}, {0, 2, 4, 6, 8, 10, 12, 14}, {1, 3, 5, 7, 9, 11, 13, 15}, {2, 3, 6, 7, 10, 11, 14, 15}, {4, 5, 6, 7, 12, 13, 14, 15}, {8, 9, 10, 11, 12, 13, 14, 15}, };
    
    ReferenceHypercube::ReferenceHypercube()
            : ReferenceGeometry(16, 4, ReferenceGeometryType::HYPERCUBE), referenceGeometryCodim1Ptr_(&ReferenceCube::Instance()), referenceGeometryCodim2Ptr_(&ReferenceSquare::Instance()), referenceGeometryCodim3Ptr_(&ReferenceLine::Instance())

    {
        name = "ReferenceHypercube";
        PointReference p0(4), p1(4), p2(4), p3(4), p4(4), p5(4), p6(4), p7(4), p8(4);
        PointReference p9(4), p10(4), p11(4), p12(4), p13(4), p14(4), p15(4);
        
        p0[0] = -1.0;
        p0[1] = -1.0;
        p0[2] = -1.0;
        p0[3] = -1.0;
        p1[0] = +1.0;
        p1[1] = -1.0;
        p1[2] = -1.0;
        p1[3] = -1.0;
        
        p2[0] = -1.0;
        p2[1] = +1.0;
        p2[2] = -1.0;
        p2[3] = -1.0;
        p3[0] = +1.0;
        p3[1] = +1.0;
        p3[2] = -1.0;
        p3[3] = -1.0;
        
        p4[0] = -1.0;
        p4[1] = -1.0;
        p4[2] = +1.0;
        p4[3] = -1.0;
        p5[0] = +1.0;
        p5[1] = -1.0;
        p5[2] = +1.0;
        p5[3] = -1.0;
        
        p6[0] = -1.0;
        p6[1] = +1.0;
        p6[2] = +1.0;
        p6[3] = -1.0;
        p7[0] = +1.0;
        p7[1] = +1.0;
        p7[2] = +1.0;
        p7[3] = -1.0;
        
        p8[0] = -1.0;
        p8[1] = -1.0;
        p8[2] = -1.0;
        p8[3] = +1.0;
        p9[0] = +1.0;
        p9[1] = -1.0;
        p9[2] = -1.0;
        p9[3] = +1.0;
        
        p10[0] = -1.0;
        p10[1] = +1.0;
        p10[2] = -1.0;
        p10[3] = +1.0;
        p11[0] = +1.0;
        p11[1] = +1.0;
        p11[2] = -1.0;
        p11[3] = +1.0;
        
        p12[0] = -1.0;
        p12[1] = -1.0;
        p12[2] = +1.0;
        p12[3] = +1.0;
        p13[0] = +1.0;
        p13[1] = -1.0;
        p13[2] = +1.0;
        p13[3] = +1.0;
        
        p14[0] = -1.0;
        p14[1] = +1.0;
        p14[2] = +1.0;
        p14[3] = +1.0;
        p15[0] = +1.0;
        p15[1] = +1.0;
        p15[2] = +1.0;
        p15[3] = +1.0;
        
        ///reference element
        
        points_[0] = p0;
        points_[1] = p1;
        points_[2] = p2;
        points_[3] = p3;
        points_[4] = p4;
        points_[5] = p5;
        points_[6] = p6;
        points_[7] = p7;
        points_[8] = p8;
        points_[9] = p9;
        points_[10] = p10;
        points_[11] = p11;
        points_[12] = p12;
        points_[13] = p13;
        points_[14] = p14;
        points_[15] = p15;
        
        mappingsCubeToHypercube_[0] = &MappingToRefCubeToHypercube0::Instance();
        mappingsCubeToHypercube_[1] = &MappingToRefCubeToHypercube1::Instance();
        mappingsCubeToHypercube_[2] = &MappingToRefCubeToHypercube2::Instance();
        mappingsCubeToHypercube_[3] = &MappingToRefCubeToHypercube3::Instance();
        mappingsCubeToHypercube_[4] = &MappingToRefCubeToHypercube4::Instance();
        mappingsCubeToHypercube_[5] = &MappingToRefCubeToHypercube5::Instance();
        mappingsCubeToHypercube_[6] = &MappingToRefCubeToHypercube6::Instance();
        mappingsCubeToHypercube_[7] = &MappingToRefCubeToHypercube7::Instance();
    }
    
    bool ReferenceHypercube::isInternalPoint(const PointReference& p) const
    {
        logger.assert(p.size()==4, "The dimension of the point is wrong");
        return ((p[0] >= -1.) && (p[0] <= 1.) && (p[1] >= -1.) && (p[1] <= 1.) && (p[2] >= -1.) && (p[2] <= 1.) && (p[3] >= -1.) && (p[3] <= 1.));
    }
    
    PointReference ReferenceHypercube::getCenter() const
    {
        return PointReference(4);
    }
    
    std::ostream& operator<<(std::ostream& os, const ReferenceHypercube& hypercube)
    {
        os << hypercube.getName() << "=( ";
        ReferenceHypercube::const_iterator cit = hypercube.points_.begin();
        ReferenceHypercube::const_iterator cend = hypercube.points_.end();
        
        for (; cit != cend; ++cit)
        {
            os << (*cit) << ' ';
        }
        os << ')' << std::endl;
        
        return os;
    }
    
    // ================================== Codimension 0 ============================================
    
    std::size_t ReferenceHypercube::getCodim0MappingIndex(const ListOfIndexesT& list1, const ListOfIndexesT& list2) const
    {
        logger(FATAL, "ReferenceCube::getCodim0MappingIndex not implemented.\n");
        return 0;
    }
    
    const MappingReferenceToReference*
    ReferenceHypercube::getCodim0MappingPtr(const std::size_t i) const
    {
        logger(FATAL, "ReferenceCube::getCodim0MappingPtr not implemented.\n");
        return 0;
    }
    
    // ================================== Codimension 1 ============================================
    
    const MappingReferenceToReference*
    ReferenceHypercube::getCodim1MappingPtr(const std::size_t faceIndex) const
    {
        logger.assert((faceIndex < 8), "ERROR: ReferenceHypercube::getCodim1MappingPtr requested face index does not exist.\n");
        return mappingsCubeToHypercube_[faceIndex];
    }
    
    const ReferenceGeometry*
    ReferenceHypercube::getCodim1ReferenceGeometry(const std::size_t faceIndex) const
    {
        logger.assert(faceIndex < 8, "You requested face %, but it does not exist", faceIndex);
        return referenceGeometryCodim1Ptr_;
    }
    
    std::vector<std::size_t> ReferenceHypercube::getCodim1EntityLocalIndices(const std::size_t i) const
    {
        logger.assert((i<8), "ERROR: ReferenceHypercube::getCodim1EntityLocalIndices requested face index does not exist.\n");
        return std::vector<std::size_t>(localNodeIndexes_[i], localNodeIndexes_[i] + 8);
        
    }
    
    // ================================== Codimension 2 ============================================
    
    const MappingReferenceToReference*
    ReferenceHypercube::getCodim2MappingPtr(const std::size_t lineIndex) const
    {
        /// TODO: Implement face to hypercube mappings.
        logger(FATAL, "ERROR: ReferenceHypercube::getCodim2MappingPtr: face to hypercube mappings not implemented.\n");
        return 0;
    }
    
    const ReferenceGeometry*
    ReferenceHypercube::getCodim2ReferenceGeometry(const std::size_t e) const
    {
        logger.assert((e < 24), "ERROR: ReferenceHypercube::getCodim2ReferenceGeometry requested side index does not exist.\n");
        return referenceGeometryCodim2Ptr_;
    }
    
    std::vector<std::size_t> ReferenceHypercube::getCodim2EntityLocalIndices(const std::size_t i) const
    {
        logger.assert((i < 24), "ReferenceHypercube::getCodim2EntityLocalIndices: not implemented.\n");
        logger(ERROR, "ReferenceHypercube::getCodim2EntityLocalIndices requested side index does not exist.\n");
        std::vector<std::size_t> dummy(1);
        return dummy;
    }
    
    // ================================== Codimension 3 ============================================
    
    std::vector<std::size_t> ReferenceHypercube::getCodim3EntityLocalIndices(const std::size_t i) const
    {
        logger.assert((i < 32), "ReferenceHypercube::getCodim3EntityLocalIndices not implemented.\n");
        logger(ERROR, "ReferenceHypercube::getCodim2EntityLocalIndices requested side index does not exist.\n");
        std::vector<std::size_t> dummy(1);
        return dummy;
    }

}
;
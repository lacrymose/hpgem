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

#ifndef ____ReferenceLine__
#define ____ReferenceLine__

#include "ReferenceGeometry.h"

#include <iostream>
#include <vector>

namespace Geometry
{
    /* Behold the reference line:
     *
     * (-1) 0---1---1 (+1)
     *
     */
    class ReferenceLine : public ReferenceGeometry
    {
        
    public:
        using ReferenceGeometry::String;
        using ListOfIndexesT = std::vector<std::size_t>;
        using Ref1ToRef1MappingT = MappingReferenceToReference<0>; // Numbers indicate dim.
        using Ref0ToRef1MappingT = MappingReferenceToReference<1>;

        static ReferenceLine& Instance()
        {
            static ReferenceLine theInstance;
            return theInstance;
        }
        
        ReferenceLine(const ReferenceLine&) = delete;
        
        //! (see ReferenceGeometry.h)
        bool isInternalPoint(const PointReference<1>&) const override final;
        
        //! Output routine.
        friend std::ostream& operator<<(std::ostream& os, const ReferenceLine& point);

        const PointReferenceBase& getCenter() const override final
        {
            return *center_;
        }

        std::size_t getNumberOfNodes() const override final
        {
            return 2;
        }

        const PointReferenceBase& getReferenceNodeCoordinate(const std::size_t& i) const override final
        {
            logger.assert(i < getNumberOfNodes(), "Asked for node %, but there are only % nodes", i, getNumberOfNodes());
            return *points_[i];
        }

        // ================================== Codimension 0 ========================================
        
        //! (see MappingCodimensions.h)
        std::size_t getCodim0MappingIndex(const ListOfIndexesT&, const ListOfIndexesT&) const override final;

        //! (see MappingCodimensions.h)
        const MappingReferenceToReference<0>* getCodim0MappingPtr(const std::size_t) const override final;

        using MappingCodimensions::getCodim0MappingPtr;

        // ================================== Codimension 1 ========================================
        
        //! (see MappingCodimensions.h)
        std::size_t getNumberOfCodim1Entities() const override final
        {
            return 2;
        }
        
        //! (see MappingCodimensions.h)
        std::vector<std::size_t> getCodim1EntityLocalIndices(const std::size_t) const override final;

        //! (see MappingCodimensions.h)
        const MappingReferenceToReference<1>* getCodim1MappingPtr(const std::size_t) const override final;

        //! (see MappingCodimensions.h)
        const ReferenceGeometry* getCodim1ReferenceGeometry(const std::size_t) const override final;

        // =============================== Refinement mappings =====================================
        
        //! Transform a reference point using refinement mapping
        void refinementTransform(int refineType, std::size_t subElementIdx, const PointReference<1>& p, PointReference<1>& pMap) const override final
        {
        }
        
        //! Transformation matrix of this refinement when located on the LEFT side
        void getRefinementMappingMatrixL(int refineType, std::size_t subElementIdx, LinearAlgebra::MiddleSizeMatrix& Q) const override final
        {
        }
        
        //! Transformation matrix of this refinement when located on the RIGHT side
        void getRefinementMappingMatrixR(int refineType, std::size_t subElementIdx, LinearAlgebra::MiddleSizeMatrix& Q) const override final
        {
        }
        
        //! Refinement mapping on codim1 for a given refinement on codim0
        //! Note: this should also applied on other dimensions
        void getCodim1RefinementMappingMatrixL(int refineType, std::size_t subElementIdx, std::size_t faLocalIndex, LinearAlgebra::MiddleSizeMatrix& Q) const override final
        {
        }
        
        //! Refinement mapping on codim1 for a given refinement on codim0
        //! Note: this should also applied on other dimensions
        void getCodim1RefinementMappingMatrixR(int refineType, std::size_t subElementIdx, std::size_t faLocalIndex, LinearAlgebra::MiddleSizeMatrix& Q) const override final
        {
        }
        
    private:
        
        ReferenceLine();

        //! Local node indexes contains the numbering of the vertex of the shape, ordered by faces.
        //! See top comment for the corresponding numbering. (In a line, the 'faces' are nodes,
        //! and nodes are just a coordinate).
        static std::size_t localNodeIndexes_[2][1];

        //! Codimension 0 mappings, from a line to a line. Used to rotate the face when the left and right elements dont think it has the same orientation
        const Ref1ToRef1MappingT* mappingsLineToLine_[2];
        
        //! Codimension 1 mappings, from a point to a line. This is the 1D face->element map
        const Ref0ToRef1MappingT* mappingsPointToLine_[2];

        //! Pointer to the Codimension 1 reference geometry, in this case, to ReferencePoint.
        ReferenceGeometry* const referenceGeometryCodim1Ptr_;

        //! List of valid quadrature rules for this reference geometry
        std::vector<QuadratureRules::GaussQuadratureRule*> lstGaussQuadratureRules_;
        
        std::vector<const PointReference<1>* > points_;

        const PointReference<1>* center_;

    };
}
#endif

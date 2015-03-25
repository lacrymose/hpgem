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

#ifndef ELEMENTGEOMETRY_H_
#define ELEMENTGEOMETRY_H_

#include <vector>
#include <iostream>
#include "Point.h"
#include "PointPhysical.h"
#include "Jacobian.h"

namespace Geometry
{
    class PointReference;
    class PointPhysical;
    class MappingReferenceToPhysical;
    class PhysicalGeometry;
    class ReferenceGeometry;
    class RefinementGeometry;
    class Jacobian;
    
    class ElementGeometry
    {
    public:
        using PointPhysicalT = PointPhysical;
        using PointReferenceT = PointReference;
        using PhysicalGeometryT = PhysicalGeometry;
        using ReferenceGeometryT = ReferenceGeometry;
        using RefinementGeometryT = RefinementGeometry;
        using MappingReferenceToPhysicalT = MappingReferenceToPhysical;
        using JacobianT = Jacobian;
        using PointIndexT = std::size_t;
        using VectorOfPhysicalPointsT = std::vector<PointPhysicalT>;
        using VectorOfPointIndexesT = std::vector<PointIndexT>;
    public:
        
        /// New style constructor with one less pass
        ElementGeometry(const VectorOfPointIndexesT& globalNodeIndexes, VectorOfPhysicalPointsT& nodes);

        /// Copy constructor
        ElementGeometry(const ElementGeometry& other);

        virtual ~ElementGeometry();

        /// Returns a pointer to the referenceToPhysicalMapping
        virtual const MappingReferenceToPhysicalT* const getReferenceToPhysicalMap() const;
        virtual MappingReferenceToPhysicalT* const getReferenceToPhysicalMap();

        /// Returns a pointer to the physicalGeometry object.
        virtual const PhysicalGeometryT* const getPhysicalGeometry() const;
        /// Returns a pointer to the physicalGeometry object.
        virtual PhysicalGeometryT* const getPhysicalGeometry();
        /// Returns a pointer to the physicalGeometry object.
        virtual std::size_t getNrOfNodes() const;
        /// Returns a pointer to the referenceGeometry object.
        virtual const ReferenceGeometryT* const getReferenceGeometry() const;
        /// Returns a pointer to the refinementGeometry object.
        virtual const RefinementGeometryT* getRefinementGeometry() const;
        /// This method gets a PointReference, which specifies a coordinate in the ReferenceGeometry,
        /// and returns a PointPhysical which is the corresponding point in the PhysicalGeometry,
        /// given the mapping.
        virtual PointPhysical referenceToPhysical(const PointReferenceT& pointReference) const;

        /// This method gets a PointReference and returns the corresponding jacobian of the
        /// referenceToPhysicalMapping.
        virtual Jacobian calcJacobian(const PointReferenceT& pointReference) const;

        void enableRefinement();

    public:
        /// Output operator.
        friend std::ostream& operator <<(std::ostream& os, const ElementGeometry& elementGeometry);
    protected:
        
        ///\brief default constructor - for use with wrapper classes (that can delegate functionality of ElementGeometry in another way)
        
        ElementGeometry()
                : referenceGeometry_(nullptr), physicalGeometry_(nullptr), referenceToPhysicalMapping_(nullptr), refinementGeometry_(nullptr)
        {
        }
        
    private:
        
        static const ReferenceGeometryT* const createReferenceGeometry(std::size_t size, std::size_t DIM);

        static PhysicalGeometryT* const createPhysicalGeometry(const VectorOfPointIndexesT& globalNodeIndexes, VectorOfPhysicalPointsT& nodes, const ReferenceGeometryT* const geo);

        static MappingReferenceToPhysicalT* const createMappings(std::size_t size, std::size_t DIM, const PhysicalGeometryT* const pGeo);

    protected:
        /// The corresponding referenceGeometry object, for integration.
        const ReferenceGeometryT* const referenceGeometry_;

        /// The physicalGeometry object contains pointers to the actual physical points, and
        /// a container of global node indexes.
        PhysicalGeometryT* const physicalGeometry_;

        /// The referenceToPhysicalMapping relates the coordinates of the reference object to the
        /// physical object; basically a matrix transformation.
        MappingReferenceToPhysicalT* const referenceToPhysicalMapping_;

        /// The corresponding refinementGeometry object
        RefinementGeometryT* refinementGeometry_;
    };

}
#endif
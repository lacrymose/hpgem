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

#ifndef MAPPINGHYPERCUBELINEAR_H_
#define MAPPINGHYPERCUBELINEAR_H_

#include "MappingReferenceToPhysical.h"
#include "Geometry/PointPhysical.h"

namespace Geometry
{
    /*!
     * "In geometry, a hypercube is an n-dimensional analogue of a square (n = 2) and a cube
     * (n = 3)" -Wikipedia.
     *
     * This templated class defines the linear mappings between hypercubes, in the corresponding
     * dimension. See the comments in the Physical<Hypercube>.cpp files to know the order of the
     * vertex of each hypercube, an order which is kept by the mappings.
     */

    template<std::size_t DIM>
    class MappingToPhysHypercubeLinear;
    
    // ~~~ Dimension 1 ~~~==========================================================================
    template<>
    class MappingToPhysHypercubeLinear<1> : public MappingReferenceToPhysical
    {
        
    public:
        MappingToPhysHypercubeLinear(const PhysicalGeometry* const &);
        PointPhysical transform(const PointReference&) const override final;
        Jacobian calcJacobian(const PointReference&) const override final;
        void reinit(const PhysicalGeometry* const) override final;
        std::size_t getTargetDimension() const override final
        {
            return 1;
        }
        
    private:
        bool isValidPoint(const PointReference&) const;
        double mid, slope;
    };
    
    // ~~~ Dimension 2 ~~~==========================================================================
    template<>
    class MappingToPhysHypercubeLinear<2> : public MappingReferenceToPhysical
    {
    public:
        // Constructor.
        MappingToPhysHypercubeLinear(const PhysicalGeometry* const &);
        PointPhysical transform(const PointReference&) const override final;
        Jacobian calcJacobian(const PointReference&) const override final;
        void reinit(const PhysicalGeometry* const) override final;
        std::size_t getTargetDimension() const override final
        {
            return 2;
        }
        
    private:
        bool isValidPoint(const PointReference&) const;
        PointPhysical a0, a1, a2, a12;
    };
    
    // ~~~ Dimension 3 ~~~==========================================================================
    template<>
    class MappingToPhysHypercubeLinear<3> : public MappingReferenceToPhysical
    {
    public:
        // Constructor.
        MappingToPhysHypercubeLinear(const PhysicalGeometry* const &);
        PointPhysical transform(const PointReference&) const override final;
        Jacobian calcJacobian(const PointReference&) const override final;
        void reinit(const PhysicalGeometry* const) override final;
        std::size_t getTargetDimension() const override final
        {
            return 3;
        }
        
    private:
        bool isValidPoint(const PointReference&) const;
        PointPhysical a0, a1, a2, a3, a12, a23, a13, a123;
    };
    
    // ~~~ Dimension 4 ~~~==========================================================================
    template<>
    class MappingToPhysHypercubeLinear<4> : public MappingReferenceToPhysical
    {
    public:
        // Constructor.
        MappingToPhysHypercubeLinear(const PhysicalGeometry* const &);
        PointPhysical transform(const PointReference&) const override final;
        Jacobian calcJacobian(const PointReference&) const override final;
        void reinit(const PhysicalGeometry* const) override final;
        std::size_t getTargetDimension() const override final
        {
            return 4;
        }
        
    private:
        bool isValidPoint(const PointReference&) const;
        PointPhysical abar, a0, a1, a2, a3, a01, a02, a03, a12, a13, a23, a012, a013, a123, a230, a0123;
    };
}
;
#endif
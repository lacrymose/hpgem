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

#ifndef MAPPINGCUBETOCUBE_C_
#define MAPPINGCUBETOCUBE_C_

#include "MappingToRefCubeToCube.hpp"
#include "Geometry/Jacobian.hpp"
#include "Geometry/PointReference.hpp"

namespace Geometry
{
    // ~~~ index 0 ~~~==============================================================================

    const MappingToRefCubeToCube0& MappingToRefCubeToCube0::Instance()
    {
        static const MappingToRefCubeToCube0 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube0::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] = p1[0];
        p2[1] = p1[1];
        p2[2] = p1[2];
    }

    void MappingToRefCubeToCube0::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) = 1.0; jacobian(0,1) = 0.0; jacobian(0,2) = 0.0;
        jacobian(1,0) = 0.0; jacobian(1,1) = 1.0; jacobian(1,2) = 0.0;
        jacobian(2,0) = 0.0; jacobian(2,1) = 0.0; jacobian(2,2) = 1.0;
    }

    MappingToRefCubeToCube0::MappingToRefCubeToCube0() { }
    MappingToRefCubeToCube0::~MappingToRefCubeToCube0() { }

    // ~~~ index 1 ~~~==============================================================================

    const MappingToRefCubeToCube1& MappingToRefCubeToCube1::Instance()
    {
        static const MappingToRefCubeToCube1 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube1::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] = p1[0];
        p2[1] = -p1[2];
        p2[2] = p1[1];
    }

    void MappingToRefCubeToCube1::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) =  1.0; jacobian(0,1) =  0.0; jacobian(0,2) =  0.0;
        jacobian(1,0) =  0.0; jacobian(1,1) =  0.0; jacobian(1,2) = -1.0;
        jacobian(2,0) =  0.0; jacobian(2,1) =  1.0; jacobian(2,2) =  0.0;
    }

    MappingToRefCubeToCube1::MappingToRefCubeToCube1() { }
    MappingToRefCubeToCube1::~MappingToRefCubeToCube1() { }

    // ~~~ index 2 ~~~==============================================================================

    const MappingToRefCubeToCube2& MappingToRefCubeToCube2::Instance()
    {
        static const MappingToRefCubeToCube2 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube2::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] =  p1[0];
        p2[1] = -p1[1];
        p2[2] = -p1[2];
    }

    void MappingToRefCubeToCube2::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) =  1.0; jacobian(0,1) =  0.0; jacobian(0,2) =  0.0;
        jacobian(1,0) =  0.0; jacobian(1,1) = -1.0; jacobian(1,2) =  0.0;
        jacobian(2,0) =  0.0; jacobian(2,1) =  0.0; jacobian(2,2) = -1.0;
    }

    MappingToRefCubeToCube2::MappingToRefCubeToCube2() { }
    MappingToRefCubeToCube2::~MappingToRefCubeToCube2() { }

    // ~~~ index 3 ~~~==============================================================================

    const MappingToRefCubeToCube3& MappingToRefCubeToCube3::Instance()
    {
        static const MappingToRefCubeToCube3 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube3::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] =  p1[0];
        p2[1] =  p1[2];
        p2[2] = -p1[1];
    }

    void MappingToRefCubeToCube3::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) =  1.0; jacobian(0,1) =  0.0; jacobian(0,2) =  0.0;
        jacobian(1,0) =  0.0; jacobian(1,1) =  0.0; jacobian(1,2) =  1.0;
        jacobian(2,0) =  0.0; jacobian(2,1) = -1.0; jacobian(2,2) =  0.0;
    }

    MappingToRefCubeToCube3::MappingToRefCubeToCube3() { }
    MappingToRefCubeToCube3::~MappingToRefCubeToCube3() { }

    // ~~~ index 4 ~~~==============================================================================

    const MappingToRefCubeToCube4& MappingToRefCubeToCube4::Instance()
    {
        static const MappingToRefCubeToCube4 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube4::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] =  p1[0];
        p2[1] =  p1[1];
        p2[2] = -p1[2];
    }

    void MappingToRefCubeToCube4::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) =  1.0; jacobian(0,1) =  0.0; jacobian(0,2) =  0.0;
        jacobian(1,0) =  0.0; jacobian(1,1) =  1.0; jacobian(1,2) =  0.0;
        jacobian(2,0) =  0.0; jacobian(2,1) =  0.0; jacobian(2,2) = -1.0;
    }

    MappingToRefCubeToCube4::MappingToRefCubeToCube4() { }
    MappingToRefCubeToCube4::~MappingToRefCubeToCube4() { }

    // ~~~ index 5 ~~~==============================================================================

    const MappingToRefCubeToCube5& MappingToRefCubeToCube5::Instance()
    {
        static const MappingToRefCubeToCube5 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube5::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] =  p1[0];
        p2[1] = -p1[1];
        p2[2] =  p1[2];
    }

    void MappingToRefCubeToCube5::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) =  1.0; jacobian(0,1) =  0.0; jacobian(0,2) =  0.0;
        jacobian(1,0) =  0.0; jacobian(1,1) = -1.0; jacobian(1,2) =  0.0;
        jacobian(2,0) =  0.0; jacobian(2,1) =  0.0; jacobian(2,2) =  1.0;
    }

    MappingToRefCubeToCube5::MappingToRefCubeToCube5() { }
    MappingToRefCubeToCube5::~MappingToRefCubeToCube5() { }

    // ~~~ index 6 ~~~==============================================================================

    const MappingToRefCubeToCube6& MappingToRefCubeToCube6::Instance()
    {
        static const MappingToRefCubeToCube6 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube6::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] =  p1[0];
        p2[1] = -p1[2];
        p2[2] = -p1[1];
    }

    void MappingToRefCubeToCube6::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) =  1.0; jacobian(0,1) =  0.0; jacobian(0,2) =  0.0;
        jacobian(1,0) =  0.0; jacobian(1,1) =  0.0; jacobian(1,2) = -1.0;
        jacobian(2,0) =  0.0; jacobian(2,1) = -1.0; jacobian(2,2) =  0.0;
    }

    MappingToRefCubeToCube6::MappingToRefCubeToCube6() { }
    MappingToRefCubeToCube6::~MappingToRefCubeToCube6() { }

    // ~~~ index 7 ~~~==============================================================================

    const MappingToRefCubeToCube7& MappingToRefCubeToCube7::Instance()
    {
        static const MappingToRefCubeToCube7 theInstance;
        return theInstance;
    }

    void MappingToRefCubeToCube7::transform(const Geometry::PointReference& p1,
                                             Geometry::PointReference& p2) const
    {
        p2[0] = p1[0];
        p2[1] = p1[2];
        p2[2] = p1[1];
    }

    void MappingToRefCubeToCube7::calcJacobian(const Geometry::PointReference&,
                                                Geometry::Jacobian& jacobian) const
    {
        jacobian(0,0) =  1.0; jacobian(0,1) =  0.0; jacobian(0,2) =  0.0;
        jacobian(1,0) =  0.0; jacobian(1,1) =  0.0; jacobian(1,2) =  1.0;
        jacobian(2,0) =  0.0; jacobian(2,1) =  1.0; jacobian(2,2) =  0.0;
    }

    MappingToRefCubeToCube7::MappingToRefCubeToCube7() { }
    MappingToRefCubeToCube7::~MappingToRefCubeToCube7() { }
};
#endif

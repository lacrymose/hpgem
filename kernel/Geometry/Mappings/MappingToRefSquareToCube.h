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

#ifndef MAPPINGSQUARETOCUBE_H_
#define MAPPINGSQUARETOCUBE_H_

#include "MappingReferenceToReference.h"

namespace Geometry
{
    /* The ordering of the vertex in a cube:
     *
     *     6o---------o7
     *     /|        /|
     *    / |       / |
     *  4o---------o5 |
     *   | 2o------|--o3
     *   | /       | /
     *   |/        |/
     *  0o---------o1
     *
     *  faces indexes:
     *              0: (0,1,2,3)
     *              1: (0,1,4,5)
     *              2: (0,2,4,6)
     *              3: (1,3,5,7)
     *              4: (2,3,6,7)
     *              5: (4,5,6,7)
     *
     * This implements the mappings of a Square [-1,1]^2 onto a cube [-1,1]^3 depending on the
     * faceindex. The mappings are defined as:
     *
     *      faceindex 0: (x,y)->(x,y,-1)
     *      faceindex 1: (x,y)->(x,-1,y)
     *      faceindex 2: (x,y)->(-1,x,y)
     *      faceindex 3: (x,y)->(1,x,y)
     *      faceindex 4: (x,y)->(x,1,y)
     *      faceindex 5: (x,y)->(x,y,1)
     *
     * The mappings are chosen to preserve the ordering of the vertices. (Ordering by coordinate).
     */

    // ~~~ index 0 ~~~==============================================================================
    class MappingToRefSquareToCube0 : public MappingReferenceToReference
    {
    public:
        static const MappingToRefSquareToCube0& Instance();
        PointReference transform(const Geometry::PointReference& p1) const override final;
        Jacobian calcJacobian(const Geometry::PointReference&) const override final;
        std::size_t getTargetDimension() const override final
        {
            return 3;
        }
    private:
        MappingToRefSquareToCube0();
        MappingToRefSquareToCube0(const MappingToRefSquareToCube0&);
        MappingToRefSquareToCube0& operator=(const MappingToRefSquareToCube0&);
    };
    
    // ~~~ index 1 ~~~==============================================================================
    
    class MappingToRefSquareToCube1 : public MappingReferenceToReference
    {
    public:
        static const MappingToRefSquareToCube1& Instance();
        PointReference transform(const Geometry::PointReference& p1) const override final;
        Jacobian calcJacobian(const Geometry::PointReference&) const override final;
        std::size_t getTargetDimension() const override final
        {
            return 3;
        }
    private:
        MappingToRefSquareToCube1();
        MappingToRefSquareToCube1(const MappingToRefSquareToCube1&);
        MappingToRefSquareToCube1& operator=(const MappingToRefSquareToCube1&);
    };
    
    // ~~~ index 2 ~~~==============================================================================
    
    class MappingToRefSquareToCube2 : public MappingReferenceToReference
    {
    public:
        static const MappingToRefSquareToCube2& Instance();
        PointReference transform(const Geometry::PointReference& p1) const override final;
        Jacobian calcJacobian(const Geometry::PointReference&) const override final;
        std::size_t getTargetDimension() const override final
        {
            return 3;
        }
    private:
        MappingToRefSquareToCube2();
        MappingToRefSquareToCube2(const MappingToRefSquareToCube2&);
        MappingToRefSquareToCube1& operator=(const MappingToRefSquareToCube2&);
    };
    
    // ~~~ index 3 ~~~==============================================================================
    
    class MappingToRefSquareToCube3 : public MappingReferenceToReference
    {
    public:
        static const MappingToRefSquareToCube3& Instance();
        PointReference transform(const Geometry::PointReference& p1) const override final;
        Jacobian calcJacobian(const Geometry::PointReference&) const override final;
        std::size_t getTargetDimension() const override final
        {
            return 3;
        }
    private:
        MappingToRefSquareToCube3();
        MappingToRefSquareToCube3(const MappingToRefSquareToCube3&);
        MappingToRefSquareToCube3& operator=(const MappingToRefSquareToCube3&);
    };
    
    // ~~~ index 4 ~~~==============================================================================
    
    class MappingToRefSquareToCube4 : public MappingReferenceToReference
    {
    public:
        static const MappingToRefSquareToCube4& Instance();
        PointReference transform(const Geometry::PointReference& p1) const override final;
        Jacobian calcJacobian(const Geometry::PointReference&) const override final;
        std::size_t getTargetDimension() const override final
        {
            return 3;
        }
    private:
        MappingToRefSquareToCube4();
        MappingToRefSquareToCube4(const MappingToRefSquareToCube4&);
        MappingToRefSquareToCube4& operator=(const MappingToRefSquareToCube4&);
    };
    
    // ~~~ index 5 ~~~==============================================================================
    
    class MappingToRefSquareToCube5 : public MappingReferenceToReference
    {
    public:
        static const MappingToRefSquareToCube5& Instance();
        PointReference transform(const Geometry::PointReference& p1) const override final;
        Jacobian calcJacobian(const Geometry::PointReference&) const override final;
        std::size_t getTargetDimension() const override final
        {
            return 3;
        }
    private:
        MappingToRefSquareToCube5();
        MappingToRefSquareToCube5(const MappingToRefSquareToCube5&);
        MappingToRefSquareToCube5& operator=(const MappingToRefSquareToCube5&);
    };

}
#endif /* MAPPINGSIMPLECUBENLINEAR_H_ */
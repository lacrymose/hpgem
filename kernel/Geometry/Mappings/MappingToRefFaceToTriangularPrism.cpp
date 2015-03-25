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

#include "MappingToRefFaceToTriangularPrism.h"
#include "Geometry/PointReference.h"
#include "Geometry/Jacobian.h"

namespace Geometry
{
    // ~~~ index 0 ~~~==============================================================================
    
    const MappingToRefFaceToTriangularPrism0& MappingToRefFaceToTriangularPrism0::Instance()
    {
        static const MappingToRefFaceToTriangularPrism0 theInstance;
        return theInstance;
    }
    
    PointReference MappingToRefFaceToTriangularPrism0::transform(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        PointReference p2(3);
        p2[0] = p1[1];
        p2[1] = p1[0];
        p2[2] = -1.0;
        return p2;
    }
    
    Jacobian MappingToRefFaceToTriangularPrism0::calcJacobian(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        Jacobian jacobian(3, 2);
        jacobian(0, 0) = 0.0;
        jacobian(1, 0) = 1.0;
        jacobian(2, 0) = 0.0;
        
        jacobian(0, 1) = 1.0;
        jacobian(1, 1) = 0.0;
        jacobian(2, 1) = 0.0;
        return jacobian;
    }
    
    MappingToRefFaceToTriangularPrism0::MappingToRefFaceToTriangularPrism0()
    {
    }
    
    // ~~~ index 1 ~~~==============================================================================
    
    const MappingToRefFaceToTriangularPrism1& MappingToRefFaceToTriangularPrism1::Instance()
    {
        static const MappingToRefFaceToTriangularPrism1 theInstance;
        return theInstance;
    }
    
    PointReference MappingToRefFaceToTriangularPrism1::transform(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        PointReference p2(3);
        p2[0] = p1[0];
        p2[1] = p1[1];
        p2[2] = 1.0;
        return p2;
    }
    
    Jacobian MappingToRefFaceToTriangularPrism1::calcJacobian(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        Jacobian jacobian(3, 2);
        jacobian(0, 0) = 1.0;
        jacobian(1, 0) = 0.0;
        jacobian(2, 0) = 0.0;
        
        jacobian(0, 1) = 0.0;
        jacobian(1, 1) = 1.0;
        jacobian(2, 1) = 0.0;
        return jacobian;
    }
    
    MappingToRefFaceToTriangularPrism1::MappingToRefFaceToTriangularPrism1()
    {
    }
    
    // ~~~ index 2 ~~~==============================================================================
    
    const MappingToRefFaceToTriangularPrism2& MappingToRefFaceToTriangularPrism2::Instance()
    {
        static const MappingToRefFaceToTriangularPrism2 theInstance;
        return theInstance;
    }
    
    PointReference MappingToRefFaceToTriangularPrism2::transform(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        PointReference p2(3);
        p2[0] = 0.0;
        p2[1] = 0.5 * (1.0 - p1[0]);
        p2[2] = p1[1];
        return p2;
    }
    
    Jacobian MappingToRefFaceToTriangularPrism2::calcJacobian(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        Jacobian jacobian(3, 2);
        jacobian(0, 0) = 0.0;
        jacobian(1, 0) = -0.5;
        jacobian(2, 0) = 0.0;
        
        jacobian(0, 1) = 0.0;
        jacobian(1, 1) = 0.0;
        jacobian(2, 1) = 1.0;
        return jacobian;
    }
    
    MappingToRefFaceToTriangularPrism2::MappingToRefFaceToTriangularPrism2()
    {
    }
    
    // ~~~ index 3 ~~~==============================================================================
    
    const MappingToRefFaceToTriangularPrism3& MappingToRefFaceToTriangularPrism3::Instance()
    {
        static const MappingToRefFaceToTriangularPrism3 theInstance;
        return theInstance;
    }
    
    PointReference MappingToRefFaceToTriangularPrism3::transform(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        PointReference p2(3);
        p2[0] = 0.5 * (1.0 + p1[0]);
        p2[1] = 0.0;
        p2[2] = p1[1];
        return p2;
    }
    
    Jacobian MappingToRefFaceToTriangularPrism3::calcJacobian(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        Jacobian jacobian(3, 2);
        jacobian(0, 0) = 0.5;
        jacobian(1, 0) = 0.0;
        jacobian(2, 0) = 0.0;
        
        jacobian(0, 1) = 0.0;
        jacobian(1, 1) = 0.0;
        jacobian(2, 1) = 1.0;
        return jacobian;
    }
    
    MappingToRefFaceToTriangularPrism3::MappingToRefFaceToTriangularPrism3()
    {
    }
    
    // ~~~ index 4 ~~~==============================================================================
    
    const MappingToRefFaceToTriangularPrism4& MappingToRefFaceToTriangularPrism4::Instance()
    {
        static const MappingToRefFaceToTriangularPrism4 theInstance;
        return theInstance;
    }
    
    PointReference MappingToRefFaceToTriangularPrism4::transform(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        PointReference p2(3);
        p2[0] = 0.5 * (1.0 - p1[0]);
        p2[1] = 0.5 * (1.0 + p1[0]);
        p2[2] = p1[1];
        return p2;
    }
    
    Jacobian MappingToRefFaceToTriangularPrism4::calcJacobian(const Geometry::PointReference& p1) const
    {
        logger.assert(p1.size()==2, "Reference point has the wrong dimension");
        Jacobian jacobian(3, 2);
        jacobian(0, 0) = -0.5;
        jacobian(1, 0) = 0.5;
        jacobian(2, 0) = 0.0;
        
        jacobian(0, 1) = 0.0;
        jacobian(1, 1) = 0.0;
        jacobian(2, 1) = 1.0;
        return jacobian;
    }
    
    MappingToRefFaceToTriangularPrism4::MappingToRefFaceToTriangularPrism4()
    {
    }

}
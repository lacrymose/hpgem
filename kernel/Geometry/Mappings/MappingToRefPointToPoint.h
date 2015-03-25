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

#ifndef MAPPINGTOREFPOINTTOPOINT_HPP_
#define MAPPINGTOREFPOINTTOPOINT_HPP_

#include "Geometry/Mappings/MappingReferenceToReference.h"
#include "Geometry/Jacobian.h"
#include "Geometry/PointReference.h"

namespace Geometry
{
    
    /*
     * maps a point to itself
     *
     * there is only one possible mapping
     *
     * this class is provided for a unified treatment of faces in the 1D case (compared to higher dimensions)
     *
     */

    class MappingToRefPointToPoint : public Geometry::MappingReferenceToReference
    {
    public:
        static const MappingToRefPointToPoint& Instance();
        PointReference transform(const Geometry::PointReference& p1) const override final;
        Jacobian calcJacobian(const Geometry::PointReference&) const override final;
        std::size_t getTargetDimension() const override final
        {
            return 0;
        }
    private:
        MappingToRefPointToPoint();
        MappingToRefPointToPoint(const MappingToRefPointToPoint&) = delete;
    };

} /* namespace Geometry */
#endif /* MAPPINGTOREFPOINTTOPOINT_HPP_ */
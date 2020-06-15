/*
 This file forms part of hpGEM. This package has been developed over a number of
 years by various people at the University of Twente and a full list of
 contributors can be found at http://hpgem.org/about-the-code/team

 This code is distributed using BSD 3-Clause License. A copy of which can found
 below.


 Copyright (c) 2014, University of Twente
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MAPPINGLINETOLINE_H_
#define MAPPINGLINETOLINE_H_

#include "MappingReferenceToReference.h"

namespace Geometry {
/*
 * The reference line:
 *
 * (-1) 0-------1 (+1)
 *
 * Linear maps of a line into itself. There are only two possible mappings:
 *
 *      index 0: x -> x
 *      index 1: x -> -x
 *
 */

// ~~~ index 0
// ~~~===========================================================================
// //
class MappingToRefLineToLine0 : public MappingReferenceToReference<0> {
   public:
    static const MappingToRefLineToLine0& Instance();
    PointReference<1> transform(
        const Geometry::PointReference<1>& p1) const final;
    Jacobian<1, 1> calcJacobian(const Geometry::PointReference<1>&) const final;
    std::size_t getTargetDimension() const final { return 1; }
    MappingToRefLineToLine0(const MappingToRefLineToLine0&) = delete;
    MappingToRefLineToLine0& operator=(const MappingToRefLineToLine0&) = delete;

   private:
    MappingToRefLineToLine0();
    std::map<const PointReference<1>*, const PointReference<1>*>
        transformedCoordinates;
};

// ~~~ index 1
// ~~~===========================================================================
// //
class MappingToRefLineToLine1 : public MappingReferenceToReference<0> {
   public:
    static const MappingToRefLineToLine1& Instance();
    PointReference<1> transform(
        const Geometry::PointReference<1>& p1) const final;
    Jacobian<1, 1> calcJacobian(const Geometry::PointReference<1>&) const final;
    std::size_t getTargetDimension() const final { return 1; }
    MappingToRefLineToLine1(const MappingToRefLineToLine1&) = delete;
    MappingToRefLineToLine1& operator=(const MappingToRefLineToLine1&) = delete;

   private:
    MappingToRefLineToLine1();
    std::map<const PointReference<1>*, const PointReference<1>*>
        transformedCoordinates;
};
}  // namespace Geometry
#endif

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
#ifndef HPGEM_KERNEL_GAUSSQUADRATURERULESFORPOINT_H
#define HPGEM_KERNEL_GAUSSQUADRATURERULESFORPOINT_H

#include "GaussQuadratureRule.h"
#include <limits>
#include "Geometry/PointReference.h"

namespace hpgem {

namespace Geometry {
class ReferencePoint;
}

namespace QuadratureRules {

///'Quadrature rule' for a single point is always exact (and just does an
/// evaluation)
class Cn0_inf_1 : public QuadratureRules::GaussQuadratureRule {
   public:
    static Cn0_inf_1& Instance() {
        static Cn0_inf_1 theInstance;
        return theInstance;
    }
    Cn0_inf_1(const Cn0_inf_1&) = delete;

    std::string getName() const final { return name_; }

    std::size_t order() const final {
        return std::numeric_limits<std::size_t>::max();
    }

    std::size_t dimension() const final { return 0; }

    std::size_t getNumberOfPoints() const final { return 1; }

    double weight(std::size_t i) const final;

    const Geometry::PointReferenceBase& getPoint(std::size_t i) const final;

    Geometry::ReferenceGeometry* forReferenceGeometry() const final {
        return refGeoPtr_;
    }

   private:
    Cn0_inf_1();

    const std::string name_;
    double weight_[1];
    Geometry::ReferenceGeometry* const refGeoPtr_;
    std::vector<Geometry::PointReference<0>> gp_;
};

} /* namespace QuadratureRules */

}  // namespace hpgem

#endif  // HPGEM_KERNEL_GAUSSQUADRATURERULESFORPOINT_H

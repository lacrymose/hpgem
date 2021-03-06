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
#ifndef HPGEM_KERNEL_ELEMENTINTEGRANDBASE_H
#define HPGEM_KERNEL_ELEMENTINTEGRANDBASE_H

#include <cstdlib>

namespace hpgem {

namespace Base {
template <std::size_t DIM>
class PhysicalElement;
}

namespace Integration {

/**
 * If you want to integrate over elements it is likely you already have the
 * functions elementIntegrand(const Base::Element*, const
 * Geometry::PointReference&, LinearAlgebra::Matrix) and elementIntegrand(const
 * Base::Element*, const Geometry::PointReference&,
 * LinearAlgebra::NumericalVector) implemented in some class already, so that
 * class can simply inherit from ElementIntegrandBase<LinearAlgebra::Matrix> and
 * ElementIntegrandBase<LinearAlgebra::NumericalVector> to signal the
 * integrators that it does so. This abstract base class is very ugly, and
 * allows templating on the return type, which is given in the final parameter,
 * and inheriting multiple instances. \deprecated Please use an std::function to
 * pass the integrand to ElementIntegral::integrate.
 */
template <class T, std::size_t DIM>
class ElementIntegrandBase {
   public:
    /// compute the contribution to the returntype of this reference point
    virtual void elementIntegrand(Base::PhysicalElement<DIM>& element,
                                  T& result) = 0;

    ElementIntegrandBase() = default;
    ElementIntegrandBase(const ElementIntegrandBase<T, DIM>& other) = delete;
};
}  // namespace Integration

}  // namespace hpgem

#endif  // HPGEM_KERNEL_ELEMENTINTEGRANDBASE_H

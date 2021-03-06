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
#include "Base/BasisFunctionSet.h"

#include "Base/BaseBasisFunction.h"
#include "Logger.h"
#include "LinearAlgebra/MiddleSizeVector.h"

namespace hpgem {

namespace Base {

// class BasisFunctionSet;
BasisFunctionSet::BasisFunctionSet(std::size_t order) : order_(order) {}

BasisFunctionSet::~BasisFunctionSet() {
    while (!vecOfBasisFcn_.empty()) {
        delete vecOfBasisFcn_.back();
        vecOfBasisFcn_.pop_back();
    }
    while (!registeredRules_.empty()) {
        registeredRules_.back()->unregisterBasisFunctionSet(this);
        registeredRules_.pop_back();
    }
}

std::size_t BasisFunctionSet::size() const { return vecOfBasisFcn_.size(); }

std::size_t BasisFunctionSet::getOrder() const { return order_; }

void BasisFunctionSet::addBasisFunction(BaseBasisFunction* bf) {
    logger.assert_debug(bf != nullptr, "Invalid basis function passed");
    vecOfBasisFcn_.push_back(bf);
    while (!registeredRules_.empty()) {
        registeredRules_.back()->unregisterBasisFunctionSet(this);
        vecOfBasisFcn_.pop_back();
    }
}
}  // namespace Base

double Base::BasisFunctionSet::eval(
    std::size_t i, QuadratureRules::GaussQuadratureRule* elementQuadratureRule,
    std::size_t quadraturePointIndex) const {
    return elementQuadratureRule->eval(this, i, quadraturePointIndex);
}

double Base::BasisFunctionSet::eval(
    std::size_t i, QuadratureRules::GaussQuadratureRule* faceQuadratureRule,
    std::size_t quadraturePointIndex,
    const Geometry::MappingReferenceToReference<1>* faceToElementMap) const {
    return faceQuadratureRule->eval(this, i, quadraturePointIndex,
                                    faceToElementMap);
}

double Base::BasisFunctionSet::evalDiv(
    std::size_t i, QuadratureRules::GaussQuadratureRule* elementQuadratureRule,
    std::size_t quadraturePointIndex) const {
    return elementQuadratureRule->evalDiv(this, i, quadraturePointIndex);
}

double Base::BasisFunctionSet::evalDiv(
    std::size_t i, QuadratureRules::GaussQuadratureRule* faceQuadratureRule,
    std::size_t quadraturePointIndex,
    const Geometry::MappingReferenceToReference<1>* faceToElementMap) const {
    return faceQuadratureRule->evalDiv(this, i, quadraturePointIndex,
                                       faceToElementMap);
}

}  // namespace hpgem

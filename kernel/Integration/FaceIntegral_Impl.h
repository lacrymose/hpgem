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
#ifndef FACEINTEGRAL_IMPL_HPP_
#define FACEINTEGRAL_IMPL_HPP_
#include "Base/ShortTermStorageFaceH1.h"

#include "QuadratureRules/GaussQuadratureRule.h"
#include "Logger.h"
#include "Base/L2Norm.h"
#include "FaceIntegrandBase.h"
#include "LinearAlgebra/Axpy.h"

namespace Integration
{
    
    template<typename ReturnTrait1>
    ReturnTrait1 FaceIntegral::integrate(Base::Face* fa, FaceIntegrandBase<ReturnTrait1>* integrand, const QuadratureRules::GaussQuadratureRule* qdrRule)
    {
        logger.assert(fa!=nullptr, "Invalid face detected");
        logger.assert(integrand!=nullptr, "Invalid integrand detected");
        //quadrature rule is allowed to be equal to nullptr!
        std::function<ReturnTrait1(const Base::Face*, const LinearAlgebra::NumericalVector&, const Geometry::PointReference&)> integrandFunc = [=](const Base::Face* face, const LinearAlgebra::NumericalVector& n, const Geometry::PointReference& p)
        {   
            ReturnTrait1 result;
            integrand->faceIntegrand(face,n,p,result);
            return result;
        };
        return integrate(fa, integrandFunc, qdrRule);
    }
    
    template<typename ReturnTrait1>
    ReturnTrait1 FaceIntegral::integrate(Base::Face* fa, std::function<ReturnTrait1(const Base::Face*, const LinearAlgebra::NumericalVector&, const Geometry::PointReference&)> integrandFunc, const QuadratureRulesT* const qdrRule)
    {
        logger.assert(fa!=nullptr, "Invalid face detected");
        //quadrature rule is allowed to be equal to nullptr!
        if (localFace_ == nullptr)
        {
            localFace_ = new Base::ShortTermStorageFaceH1(fa->getGaussQuadratureRule()->dimension() + 1);
        }
        *localFace_ = *fa;
        const QuadratureRulesT * const qdrRuleLoc = (qdrRule == nullptr ? localFace_->getGaussQuadratureRule() : qdrRule);
        
        // check whether the GaussIntegrationRule is actually for the
        // Element's ReferenceGeometry
        logger.assert((qdrRuleLoc->forReferenceGeometry() == localFace_->getReferenceGeometry()), "FaceIntegral: " + qdrRuleLoc->getName() + " rule is not for THIS ReferenceGeometry!");
        
        // value returned by the integrand
        ReturnTrait1 value, result;
        
        // number of Gauss quadrature points
        std::size_t nrOfPoints = qdrRuleLoc->nrOfPoints();
        
        // Gauss quadrature point
        Geometry::PointReference p = qdrRuleLoc->getPoint(0);
        
        LinearAlgebra::NumericalVector Normal = localFace_->getNormalVector(p);
        
        // first Gauss point;
        result = integrandFunc(localFace_, Normal, p);
        result *= (qdrRuleLoc->weight(0) * Base::L2Norm(Normal));
        
        // next Gauss points
        for (std::size_t i = 1; i < nrOfPoints; ++i)
        {
            p = qdrRuleLoc->getPoint(i);
            Normal = localFace_->getNormalVector(p);
            value = integrandFunc(localFace_, Normal, p);
            
            //Y = alpha * X + Y
            LinearAlgebra::axpy(qdrRuleLoc->weight(i) * Base::L2Norm(Normal), value, result);
            
        }
        return result;
    } // function
    
    template<typename IntegrandType>
    IntegrandType FaceIntegral::referenceFaceIntegral(const QuadratureRules::GaussQuadratureRule *ptrQdrRule, std::function<IntegrandType(const Geometry::PointReference &)> integrandFunction)
    {
        std::size_t numOfPoints = ptrQdrRule->nrOfPoints();
        std::size_t iPoint = 0; // Index for the quadrature points.
        
        Geometry::PointReference pRef = ptrQdrRule->getPoint(iPoint);
        IntegrandType integral(ptrQdrRule->weight(iPoint) * integrandFunction(pRef));
        for (iPoint = 1; iPoint < numOfPoints; iPoint++)
        {
            pRef = ptrQdrRule->getPoint(iPoint);
            LinearAlgebra::axpy(ptrQdrRule->weight(iPoint), integrandFunction(pRef), integral);
        }
        return integral;
    }

}

#endif /* FACEINTEGRAL_IMPL_HPP_ */
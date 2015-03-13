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

#include "Base/ShortTermStorageFaceHCurl.h"
#include "Base/ShortTermStorageElementHCurl.h"
#include "Geometry/PhysicalGeometry.h"
#include "L2Norm.h"
#include "Geometry/PointPhysical.h"
#include "FaceCacheData.h"
#include "ElementCacheData.h"
#include "Geometry/PointReference.h"
#include "Geometry/ReferenceGeometry.h"
#include "Geometry/PointPhysical.h"

/*
 void Base::ShortTermStorageFaceHcurl::computeData() {
 ShortTermStorageFaceBase::computeData();
 int n=face_->getNrOfBasisFunctions();
 LinearAlgebra::NumericalVector dummy(3), normedNormal(3), ret;//dummy gives basisFunctionValues for given point and normedNormal gives normalised unit normal vector
 basisFunctionValues_.resize(n);
 basisFunctionCurlValues_.resize(n);
 basisFunctionsTimesNormal_.resize(n);
 static ShortTermStorageElementBase* elementwrapper(NULL);
 if(elementwrapper==NULL){
 elementwrapper=new ShortTermStorageElementHcurl(currentPoint_.size()+1);
 }else if(elementwrapper->getPhysicalGeometry()->getNodePtr(0)->size()!=currentPoint_.size()+1){
 delete elementwrapper;
 elementwrapper=new ShortTermStorageElementHcurl(currentPoint_.size()+1);
 }
 
 
 
 normedNormal[0] = (normal_ * (1/Base::L2Norm(normal_)))[0];
 normedNormal[1] = (normal_ * (1/Base::L2Norm(normal_)))[1];
 normedNormal[2] = (normal_ * (1/Base::L2Norm(normal_)))[2];
 
 int leftFunctions = getPtrElementLeft()->getNrOfBasisFunctions();
 *elementwrapper = *getPtrElementLeft();
 Geometry::PointReference pElement(currentPoint_.size()+1);
 mapRefFaceToRefElemL(currentPoint_,pElement);
 for(int i=0;i<leftFunctions;++i)
 {
 
 Geometry::Jacobian Inverse(3, 3), jacobian(3, 3);
 elementwrapper->calcJacobian(pElement, jacobian);
 
 basisFunctionCurlValues_[i].resize(3);
 elementwrapper->basisFunctionCurl(i,pElement,basisFunctionCurlValues_[i]);
 basisFunctionCurlValues_[i] = (jacobian / (std::abs(jacobian.determinant()))) * basisFunctionCurlValues_[i];
 
 
 basisFunctionValues_[i].resize(3);
 elementwrapper->basisFunction(i, pElement, basisFunctionValues_[i]);//basisFunctionValues_[i][0] = elementwrapper->basisFunction(i, pElement);
 //jac_.inverse(jac_);
 //Inverse stores the inverted and transposed matrix
 
 Inverse(0,0) = jacobian(1,1) * jacobian(2,2) - jacobian(1,2) * jacobian(2,1);
 Inverse(1,0) = jacobian(2,1) * jacobian(0,2) - jacobian(2,2) * jacobian(0,1);
 Inverse(2,0) = jacobian(0,1) * jacobian(1,2) - jacobian(0,2) * jacobian(1,1);
 Inverse(0,1) = jacobian(1,2) * jacobian(2,0) - jacobian(1,0) * jacobian(2,2);
 Inverse(1,1) = jacobian(2,2) * jacobian(0,0) - jacobian(2,0) * jacobian(0,2);
 Inverse(2,1) = jacobian(0,2) * jacobian(1,0) - jacobian(0,0) * jacobian(1,2);
 Inverse(0,2) = jacobian(1,0) * jacobian(2,1) - jacobian(1,1) * jacobian(2,0);
 Inverse(1,2) = jacobian(2,0) * jacobian(0,1) - jacobian(2,1) * jacobian(0,0);
 Inverse(2,2) = jacobian(0,0) * jacobian(1,1) - jacobian(0,1) * jacobian(1,0);
 
 Inverse /= std::abs(jacobian.determinant());
 
 basisFunctionValues_[i] = Inverse  * basisFunctionValues_[i];//basisFunctionValues_[i][0] = jac_ * basisFunctionValues_[i][0];
 dummy = basisFunctionValues_[i];
 
 //this should change to accommodate vector product of normal with phi
 ret.resize(3);
 basisFunctionsTimesNormal_[i].resize(currentPoint_.size()+1);
 ret[0] = normedNormal[1] * dummy[2] - normedNormal[2] * dummy[1];
 ret[1] = normedNormal[2] * dummy[0] - normedNormal[0] * dummy[2];
 ret[2] = normedNormal[0] * dummy[1] - normedNormal[1] * dummy[0];
 
 basisFunctionsTimesNormal_[i] = ret;
 
 
 }
 if(n>leftFunctions){
 *elementwrapper=*getPtrElementRight();
 mapRefFaceToRefElemR(currentPoint_,pElement);
 }
 for(int i=leftFunctions;i<n;++i)
 {
 
 
 Geometry::Jacobian Inverse(3, 3), jacobian(3, 3);
 elementwrapper->calcJacobian(pElement, jacobian);
 basisFunctionCurlValues_[i].resize(3);
 elementwrapper->basisFunctionCurl(i-leftFunctions,pElement,basisFunctionCurlValues_[i]);
 basisFunctionCurlValues_[i] = (jacobian / (std::abs(jacobian.determinant()))) * basisFunctionCurlValues_[i];

 //jac_.inverse(jac_);
 //Inverse stores the inverted and transposed matrix
 
 Inverse(0,0) = jacobian(1,1) * jacobian(2,2) - jacobian(1,2) * jacobian(2,1);
 Inverse(1,0) = jacobian(2,1) * jacobian(0,2) - jacobian(2,2) * jacobian(0,1);
 Inverse(2,0) = jacobian(0,1) * jacobian(1,2) - jacobian(0,2) * jacobian(1,1);
 Inverse(0,1) = jacobian(1,2) * jacobian(2,0) - jacobian(1,0) * jacobian(2,2);
 Inverse(1,1) = jacobian(2,2) * jacobian(0,0) - jacobian(2,0) * jacobian(0,2);
 Inverse(2,1) = jacobian(0,2) * jacobian(1,0) - jacobian(0,0) * jacobian(1,2);
 Inverse(0,2) = jacobian(1,0) * jacobian(2,1) - jacobian(1,1) * jacobian(2,0);
 Inverse(1,2) = jacobian(2,0) * jacobian(0,1) - jacobian(2,1) * jacobian(0,0);
 Inverse(2,2) = jacobian(0,0) * jacobian(1,1) - jacobian(0,1) * jacobian(1,0);
 Inverse /= std::abs(jacobian.determinant());
 
 basisFunctionValues_[i].resize(3);
 elementwrapper->basisFunction(i-leftFunctions, pElement, basisFunctionValues_[i]); //basisFunctionValues_[i][0] = elementwrapper->basisFunction(i-leftFunctions,pElement);
 ret.resize(3);
 basisFunctionValues_[i] = Inverse * basisFunctionValues_[i];//basisFunctionValues_[i][0] = jac_ * basisFunctionValues_[i][0];
 dummy = basisFunctionValues_[i];
 dummy *= -1;
 
 //this should change to accommodate vector product of normal with phi, using concept from line 169-171 from Fill Matrices
 
 basisFunctionsTimesNormal_[i].resize(currentPoint_.size()+1);
 ret[0] = normedNormal[1] * dummy[2] - normedNormal[2] * dummy[1];
 ret[1] = normedNormal[2] * dummy[0] - normedNormal[0] * dummy[2];
 ret[2] = normedNormal[0] * dummy[1] - normedNormal[1] * dummy[0];
 
 basisFunctionsTimesNormal_[i] = ret;
 
 }
 }
 */
void Base::ShortTermStorageFaceHcurl::computeData()
{
    logger(DEBUG, "Called ShortTermStorageFaceHcurl computeData");
    ShortTermStorageFaceBase::computeData();
    logger(DEBUG, "Called ShortTermStorageFaceBase computeData");
    
    std::size_t DIM = currentPoint_.size() + 1;
    logger(DEBUG, "Dimension of the system: %", DIM);
    Geometry::Jacobian jacobian(DIM, DIM);
    
    std::size_t nLeft(getPtrElementLeft()->getNrOfBasisFunctions());
    logger(DEBUG, "n initialised with value %", nLeft);
    
    std::size_t nTotal(face_->getNrOfBasisFunctions());
    logger(DEBUG, "M initialised with value %", nTotal);
    
    basisFunctionValues_.resize(nTotal);
    basisFunctionCurlValues_.resize(nTotal);
    basisFunctionsTimesNormal_.resize(nTotal);
    
    LinearAlgebra::NumericalVector normedNormal = normal_ / Base::L2Norm(normal_);
    
    Geometry::PointReference pElement(DIM);
    logger(DEBUG, "Loop to be started");
    for (std::size_t i = 0; i < nTotal; ++i)
    {
        logger(DEBUG, "i: %", i);
        
        basisFunctionValues_[i].resize(DIM);
        basisFunctionCurlValues_[i].resize(DIM);
        basisFunctionsTimesNormal_[i].resize(DIM);
        
        face_->basisFunction(i, currentPoint_, basisFunctionValues_[i]);
        basisFunctionCurlValues_[i] = face_->basisFunctionCurl(i, currentPoint_);
        
        if (i < nLeft)
        {
            logger(DEBUG, "True");
            pElement = mapRefFaceToRefElemL(currentPoint_);
            jacobian = getPtrElementLeft()->calcJacobian(pElement);
        }
        else
        {
            logger(DEBUG, "False");
            pElement = mapRefFaceToRefElemR(currentPoint_);
            jacobian = getPtrElementRight()->calcJacobian(pElement);
        }
        
        //Curl of Phi
        basisFunctionCurlValues_[i] = (jacobian / (std::abs(jacobian.determinant()))) * basisFunctionCurlValues_[i];
        
        //Now jacobian contains the inverse of previous self
        jacobian = jacobian.inverse();
        for (int j = 0; j < DIM; ++j)
        {
            for (int k = 0; k < j; ++k)
            {
                std::swap(jacobian(j, k), jacobian(k, j));
            }
            
        }
        
        //Phi
        basisFunctionValues_[i] = jacobian * basisFunctionValues_[i];
        
        //Vector production of Normal and Vector basis Function
        basisFunctionsTimesNormal_[i][0] = normedNormal[1] * basisFunctionValues_[i][2] - normedNormal[2] * basisFunctionValues_[i][1];
        basisFunctionsTimesNormal_[i][1] = normedNormal[2] * basisFunctionValues_[i][0] - normedNormal[0] * basisFunctionValues_[i][2];
        basisFunctionsTimesNormal_[i][2] = normedNormal[0] * basisFunctionValues_[i][1] - normedNormal[1] * basisFunctionValues_[i][0];
        if(i > nLeft)
        {
            basisFunctionsTimesNormal_[i] *= -1.;
        }
        
    }
    logger(DEBUG, "End");
}

void Base::ShortTermStorageFaceHcurl::basisFunction(std::size_t i, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret)
{
    if (!(currentPoint_ == p))
    {
        currentPoint_ = p;
        computeData();
    }
    ret = basisFunctionValues_[i];
}

void Base::ShortTermStorageFaceHcurl::basisFunction(std::size_t i, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret) const
{
    ret = basisFunctionValues_[i];
    if (!(currentPoint_ == p))
    {
        logger(WARN, "Warning: you are using slow data access");
        face_->basisFunction(i, p, ret);
        //apply the coordinate transformation
        if(i < getPtrElementLeft()->getNrOfBasisFunctions())
        {
            Geometry::Jacobian jac = getPtrElementLeft()->calcJacobian(mapRefFaceToRefElemL(p));
            jac = jac.inverse();
            for(std::size_t i = 0; i < p.size() + 1; ++i)
            {
                for(std::size_t j=0; j < i; ++j)
                {
                    std::swap(jac(i, j), jac(j, i));
                }
            }
            ret = jac * ret;
        }
        else
        {
            Geometry::Jacobian jac = getPtrElementRight()->calcJacobian(mapRefFaceToRefElemR(p));
            jac = jac.inverse();
            for(std::size_t i = 0; i < p.size(); ++i)
            {
                for(std::size_t j=0; j < i; ++j)
                {
                    std::swap(jac(i, j), jac(j, i));
                }
            }
            ret = jac * ret;
        }
    }
}

void Base::ShortTermStorageFaceHcurl::basisFunctionNormal(std::size_t i, const LinearAlgebra::NumericalVector& normal, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret)
{
    if (!(currentPoint_ == p))
    {
        currentPoint_ = p;
        computeData();
    }
    
    ret = basisFunctionsTimesNormal_[i];
}

void Base::ShortTermStorageFaceHcurl::basisFunctionNormal(std::size_t i, const LinearAlgebra::NumericalVector& normal, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret) const
{
    ret = basisFunctionsTimesNormal_[i]; // check how to get vector product of normal and basis function
    if (!(currentPoint_ == p))
    {
        logger(WARN, "Warning: you are using slow data access");
        LinearAlgebra::NumericalVector dummy(ret);
        face_->basisFunction(i, p, dummy);
        //apply the coordinate transformation
        if(i < getPtrElementLeft()->getNrOfBasisFunctions())
        {
            Geometry::Jacobian jac = getPtrElementLeft()->calcJacobian(mapRefFaceToRefElemL(p));
            jac = jac.inverse();
            for(std::size_t i = 0; i < p.size() + 1; ++i)
            {
                for(std::size_t j=0; j < i; ++j)
                {
                    std::swap(jac(i, j), jac(j, i));
                }
            }
            LinearAlgebra::NumericalVector normedNormal = getNormalVector(p) / Base::L2Norm(getNormalVector(p));
            dummy = jac * dummy;
        
            ret[0] = normedNormal[1] * dummy[2] - normedNormal[2] * dummy[1];
            ret[1] = normedNormal[2] * dummy[0] - normedNormal[0] * dummy[2];
            ret[2] = normedNormal[0] * dummy[1] - normedNormal[1] * dummy[0];
        }
        else
        {
            Geometry::Jacobian jac = getPtrElementRight()->calcJacobian(mapRefFaceToRefElemR(p));
            jac = jac.inverse();
            for(std::size_t i = 0; i < p.size(); ++i)
            {
                for(std::size_t j=0; j < i; ++j)
                {
                    std::swap(jac(i, j), jac(j, i));
                }
            }
            LinearAlgebra::NumericalVector normedNormal = getNormalVector(p) / Base::L2Norm(getNormalVector(p));
            dummy = jac * dummy * -1.;
        
            ret[0] = normedNormal[1] * dummy[2] - normedNormal[2] * dummy[1];
            ret[1] = normedNormal[2] * dummy[0] - normedNormal[0] * dummy[2];
            ret[2] = normedNormal[0] * dummy[1] - normedNormal[1] * dummy[0];
        }
    }
}

void Base::ShortTermStorageFaceHcurl::basisFunctionCurl(std::size_t i, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret)
{
    if (!(currentPoint_ == p))
    {
        currentPoint_ = p;
        computeData();
    }
    ret = basisFunctionCurlValues_[i];
}

void Base::ShortTermStorageFaceHcurl::basisFunctionCurl(std::size_t i, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret) const
{
    ret = basisFunctionCurlValues_[i];
    if (!(currentPoint_ == p))
    {
        logger(WARN, "Warning: you are using slow data access");
        ret = face_->basisFunctionCurl(i, p);
        //apply the coordinate transformation
        if(i < getPtrElementLeft()->getNrOfBasisFunctions())
        {
            Geometry::Jacobian jac = getPtrElementLeft()->calcJacobian(mapRefFaceToRefElemL(p));
            ret = jac * ret / jac.determinant();
            
        }
        else
        {
            Geometry::Jacobian jac = getPtrElementRight()->calcJacobian(mapRefFaceToRefElemR(p));
            ret = jac * ret / jac.determinant();
            
        }
    }
}


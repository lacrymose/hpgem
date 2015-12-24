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

#ifndef PHYSYCALELEMENT_H_
#define PHYSYCALELEMENT_H_

#include <cstdlib>
#include "Geometry/PointReference.h"
#include "Geometry/PointPhysical.h"
#include "Geometry/Jacobian.h"
#include "CoordinateTransformation.h"
#include "H1ConformingTransformation.h"

namespace Base
{
    template<std::size_t DIM>
    class PhysicalFace;

    //class is final as a reminder that there is no virtual destructor
    //note that none of the functions in here is marked const, because a PhysicalElement reserves the right to alter its internal state to optimize future repeated calls
    //note that names in this class match the names in Element unless this makes no sense
    //when you use a physical element in the kernel be careful to avoid infinite recursion
    ///\todo generalize implementation to support the cached data
    //\todo move implementation of methods to PhysicalElement_Impl.h
    template<std::size_t DIM>
    class PhysicalElement final
    {
    public:
        PhysicalElement()
            :  transform_((new H1ConformingTransformation<DIM>())), hasPointReference(false), hasElement(false) // other data will get initialized when we have more info
        {
                hasElementMatrix = false;
                hasElementVector = false;
        }

        PhysicalElement(const PhysicalElement& other) = delete;
        PhysicalElement(PhysicalElement&& other) = delete;

        ///value of basis function i at the current reference point
        double basisFunction(std::size_t i);

        ///derivative of basis function i at the current reference point
        const LinearAlgebra::SmallVector<DIM>& basisFunctionDeriv(std::size_t i);


        ///value of basis function i at the current reference point
        void basisFunction(std::size_t i, LinearAlgebra::SmallVector<DIM>& result);

        ///curl of basis function i at the current reference point
        const LinearAlgebra::SmallVector<DIM>& basisFunctionCurl(std::size_t i);

        ///divergence of basis function i at the current reference point
        const double& basisFunctionDiv(std::size_t i);


        ///value of the solution at the current reference point at time level 0
        const LinearAlgebra::MiddleSizeVector& getSolution();

        ///derivative of the solution at the current reference point at time level 0
        const std::vector<LinearAlgebra::SmallVector<DIM> >& getSolutionDeriv();


        ///value of the solution at the current reference point at time level 0
        void getSolution(std::vector<LinearAlgebra::SmallVector<DIM> >& result);

        ///curl of the solution at the current reference point at time level 0
        const std::vector<LinearAlgebra::SmallVector<DIM> >& getSolutionCurl();

        ///divergence of the solution at the current reference point at time level 0
        const LinearAlgebra::MiddleSizeVector& getSolutionDiv();


        ///the current reference point
        const Geometry::PointReference<DIM>& getPointReference();

        ///the current physical point
        const Geometry::PointPhysical<DIM>& getPointPhysical();


        ///the Jacobian of the coordinate transformation
        const Geometry::Jacobian<DIM, DIM>& getJacobian();

        ///the transpose of the inverse of the Jacobian of the coordinate transformation
        const Geometry::Jacobian<DIM, DIM>& getInverseTransposeJacobian();

        ///the transpose of the Jacobian of the coordinate transformation
        const Geometry::Jacobian<DIM, DIM>& getTransposeJacobian();

        ///the absolute value of the determinant of the Jacobian of the coordinate transformation
        double getJacobianAbsDet();

        ///the determinant of the Jacobian of the coordinate transformation
        double getJacobianDet();
        
        ///a middle size square matrix of size nBasisFunctions x nUnknowns
        ///\details this gets zeroed out every time the reference point is changed and is only resized by the physical element upon construction, so this could also be used for matrixes of different size
        LinearAlgebra::MiddleSizeMatrix& getResultMatrix();

        ///a middle size vector of size nBasisFunctions x nUnknowns
        ///\details this gets zeroed out every time the reference point is changed and is only resized by the physical element upon construction, so this could also be used for vectors of different size
        LinearAlgebra::MiddleSizeVector& getResultVector();

        ///the element (elements have extra functions for users that need them)
        const Base::Element* getElement();

        ///\deprecated Does not conform naming conventions, use getNumberOfBasisFunctions instead
        std::size_t getNumOfBasisFunctions()
        {
            return getNumberOfBasisFunctions();
        }
        
        ///the number of basis functions that are nonzero in the element
        std::size_t getNumberOfBasisFunctions()
        {
            return theElement_->getNumberOfBasisFunctions();
        }

        ///\deprecated Does not conform naming conventions, use getNumberOfUnknowns instead
        std::size_t getNumOfUnknowns()
        {
            return getNumberOfUnknowns();
        }
        
        ///the number of unknowns present in the problem
        std::size_t getNumberOfUnknowns()
        {
            return theElement_->getNumberOfUnknowns();
        }

        ///the id of the element
        std::size_t getID()
        {
            return theElement_->getID();
        }

        ///combine a function index and a variable index to a single index that can be used for indexing matrices or vectors
        std::size_t convertToSingleIndex(std::size_t functionId, std::size_t variableId)
        {
            //currently calling the function is too fast to be worth storing the variable
            return theElement_->convertToSingleIndex(functionId, variableId);
        }

        ///the transformation that was used to get from the reference element to the physical element (should only be needed internally)
        const Base::CoordinateTransformation<DIM>* getTransformation();

        ///setters should only be needed internally
        void setPointReference(const Geometry::PointReference<DIM>& point);
        ///setters should only be needed internally
        void setElement(const Element* element);
        ///setters should only be needed internally
        void setTransformation(std::shared_ptr<Base::CoordinateTransformation<DIM> >& transform);

    private:

        const Base::Element* theElement_;
        const Geometry::PointReference<DIM>* pointReference_;
        std::shared_ptr<Base::CoordinateTransformation<DIM> > transform_;

        std::vector<double> basisFunctionValue;
        std::vector<LinearAlgebra::SmallVector<DIM> > vectorBasisFunctionValue;
        std::vector<LinearAlgebra::SmallVector<DIM> > basisFunctionDeriv_;
        std::vector<LinearAlgebra::SmallVector<DIM> > basisFunctionCurl_;
        std::vector<double> basisFunctionDiv_;

        LinearAlgebra::MiddleSizeVector solution;
        std::vector<LinearAlgebra::SmallVector<DIM> > vectorSolution;
        std::vector<LinearAlgebra::SmallVector<DIM> > solutionDeriv;
        std::vector<LinearAlgebra::SmallVector<DIM> > solutionCurl;
        LinearAlgebra::MiddleSizeVector solutionDiv;

        Geometry::PointPhysical<DIM> pointPhysical;
        Geometry::Jacobian<DIM, DIM> jacobian, transposeJacobian, inverseTransposeJacobian;
        double jacobianAbsDet;
        double jacobianDet;

        LinearAlgebra::MiddleSizeMatrix resultMatrix;
        LinearAlgebra::MiddleSizeVector resultVector;

        bool hasPointReference, hasElement;

        bool hasElementMatrix, hasElementVector;
        //did we already compute this?
        bool hasFunctionValue, hasVectorFunctionValue, hasFunctionDeriv, hasFunctionCurl, hasFunctionDiv;
        bool hasSolution, hasVectorSolution, hasSolutionDeriv, hasSolutionCurl, hasSolutionDiv;
        bool hasPointPhysical, hasJacobian, hasTransposeJacobian, hasInverseTransposeJacobian, hasJacobianDet, hasJacobianAbsDet;
    };

    template<std::size_t DIM>
    inline double PhysicalElement<DIM>::basisFunction(std::size_t i)
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasFunctionValue)
        {
            return basisFunctionValue[i];
        }
        else
        {
            hasFunctionValue = true;
            for(std::size_t j = 0; j < theElement_->getNumberOfBasisFunctions(); ++j)
            {
                basisFunctionValue[j] = transform_->transform(theElement_->basisFunction(j, *pointReference_), *this);
            }
            return basisFunctionValue[i];
        }
    }

}
    
    template<std::size_t DIM>
    inline const LinearAlgebra::SmallVector<DIM>& Base::PhysicalElement<DIM>::basisFunctionDeriv(std::size_t i)
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasFunctionDeriv)
        {
            return basisFunctionDeriv_[i];
        }
        else
        {
            hasFunctionDeriv = true;
            for(std::size_t j = 0; j < theElement_->getNumberOfBasisFunctions(); ++j)
            {
                basisFunctionDeriv_[j] = transform_->transformDeriv(theElement_->basisFunctionDeriv(j, *pointReference_), *this);
            }
            return basisFunctionDeriv_[i];
        }
    }
    
    template<std::size_t DIM>
    inline void Base::PhysicalElement<DIM>::basisFunction(std::size_t i, LinearAlgebra::SmallVector<DIM>& result)
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasVectorFunctionValue)
        {
            result = vectorBasisFunctionValue[i];
            return;
        }
        else
        {
            hasVectorFunctionValue = true;
            for(std::size_t j = 0; j < theElement_->getNumberOfBasisFunctions(); ++j)
            {
                theElement_->basisFunction(j, *pointReference_, vectorBasisFunctionValue[j]);
                vectorBasisFunctionValue[j] = transform_->transform(vectorBasisFunctionValue[j], *this);
            }
            result = vectorBasisFunctionValue[i];
            return;
        }
    }
    
    template<std::size_t DIM>
    inline const LinearAlgebra::SmallVector<DIM>& Base::PhysicalElement<DIM>::basisFunctionCurl(std::size_t i)
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasFunctionCurl)
        {
            return basisFunctionCurl_[i];
        }
        else
        {
            hasFunctionCurl = true;
            for(std::size_t j = 0; j < theElement_->getNumberOfBasisFunctions(); ++j)
            {
                basisFunctionCurl_[j] = transform_->transformCurl(theElement_->basisFunctionCurl(j, *pointReference_), *this);
            }
            return basisFunctionCurl_[i];
        }
    }
    
    template<std::size_t DIM>
    inline const double& Base::PhysicalElement<DIM>::basisFunctionDiv(std::size_t i)
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasFunctionDiv)
        {
            return basisFunctionDiv_[i];
        }
        else
        {
            hasFunctionDiv = true;
            for(std::size_t j = 0; j < theElement_->getNumberOfBasisFunctions(); ++j)
            {
                basisFunctionDiv_[j] = transform_->transformDiv(theElement_->basisFunctionDiv(j, *pointReference_), *this);
            }
            return basisFunctionDiv_[i];
        }
    }

    template<std::size_t DIM>
    inline const LinearAlgebra::MiddleSizeVector& Base::PhysicalElement<DIM>::getSolution()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasSolution)
        {
            return solution;
        }
        else
        {
            hasSolution = true;
            solution = theElement_->getSolution(0, *this);
            return solution;
        }
    }
    
    template<std::size_t DIM>
    inline const std::vector<LinearAlgebra::SmallVector<DIM> >& Base::PhysicalElement<DIM>::getSolutionDeriv()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasSolutionDeriv)
        {
            return solutionDeriv;
        }
        else
        {
            hasSolutionDeriv = true;
            solutionDeriv = theElement_->getSolutionGradient(0, *this);
            return solutionDeriv;
        }
    }
    
    template<std::size_t DIM>
    inline void Base::PhysicalElement<DIM>::getSolution(std::vector<LinearAlgebra::SmallVector<DIM> >& result)
    {
        logger(ERROR, "not supported by element yet");
    }
    
    template<std::size_t DIM>
    inline const std::vector<LinearAlgebra::SmallVector<DIM> >& Base::PhysicalElement<DIM>::getSolutionCurl()
    {
        logger(ERROR, "not supported by element yet");
        return std::vector<LinearAlgebra::SmallVector<DIM> >();
    }
    
    template<std::size_t DIM>
    inline const LinearAlgebra::MiddleSizeVector& Base::PhysicalElement<DIM>::getSolutionDiv()
    {
        logger(ERROR, "not supported by element yet");
        //just inlining a default-constructed vector complains about stack return despite being dead code
        static LinearAlgebra::MiddleSizeVector dummy;
        return dummy;
    }

    template<std::size_t DIM>
    inline const Geometry::PointReference<DIM>& Base::PhysicalElement<DIM>::getPointReference()
    {
        logger.assert(hasPointReference, "Need a location to evaluate the data");
        return *pointReference_;
    }
    
    template<std::size_t DIM>
    inline const Geometry::PointPhysical<DIM>& Base::PhysicalElement<DIM>::getPointPhysical()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasPointPhysical)
        {
            return pointPhysical;
        }
        else
        {
            hasPointPhysical = true;
            pointPhysical = theElement_->referenceToPhysical(*pointReference_);
            return pointPhysical;
        }
    }
    
    template<std::size_t DIM>
    inline const Geometry::Jacobian<DIM, DIM>& Base::PhysicalElement<DIM>::getJacobian()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasJacobian)
        {
            return jacobian;
        }
        else
        {
            hasJacobian = true;
            jacobian = theElement_->calcJacobian(*pointReference_);
            return jacobian;
        }
    }
    
    template<std::size_t DIM>
    inline const Geometry::Jacobian<DIM, DIM>& Base::PhysicalElement<DIM>::getInverseTransposeJacobian()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasInverseTransposeJacobian)
        {
            return inverseTransposeJacobian;
        }
        else
        {
            hasInverseTransposeJacobian = true;
            inverseTransposeJacobian = getTransposeJacobian().inverse();
            return inverseTransposeJacobian;
        }
    }
    
    template<std::size_t DIM>
    inline const Geometry::Jacobian<DIM, DIM>& Base::PhysicalElement<DIM>::getTransposeJacobian()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasTransposeJacobian)
        {
            return transposeJacobian;
        }
        else
        {
            hasTransposeJacobian = true;
            transposeJacobian = getJacobian().transpose();
            return transposeJacobian;
        }
    }
    
    template<std::size_t DIM>
    inline double Base::PhysicalElement<DIM>::getJacobianAbsDet()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasJacobianAbsDet)
        {
            return jacobianAbsDet;
        }
        else
        {
            hasJacobianAbsDet = true;
            jacobianAbsDet = std::abs(getJacobianDet());
            return jacobianAbsDet;
        }
    }

    template<std::size_t DIM>
    inline double Base::PhysicalElement<DIM>::getJacobianDet()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        if(hasJacobianDet)
        {
            return jacobianDet;
        }
        else
        {
            hasJacobianDet = true;
            jacobianDet = getJacobian().determinant();
            return jacobianDet;
        }
    }


    template<std::size_t DIM>
    inline LinearAlgebra::MiddleSizeMatrix& Base::PhysicalElement<DIM>::getResultMatrix()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        logger.assert(hasElementMatrix, "Can only provide the matrix once per coordinate");
        hasElementMatrix = false;
        return resultMatrix;
    }
    
    template<std::size_t DIM>
    inline LinearAlgebra::MiddleSizeVector& Base::PhysicalElement<DIM>::getResultVector()
    {
        logger.assert(hasPointReference && hasElement, "Need a location to evaluate the data");
        logger.assert(hasElementVector, "Can only provide the vector once per coordinate");
        hasElementVector = false;
        return resultVector;
    }
    
    template<std::size_t DIM>
    inline const Base::Element* Base::PhysicalElement<DIM>::getElement()
    {
        logger.assert(hasElement, "Need a location to evaluate the data");
        return theElement_;
    }
    
    template<std::size_t DIM>
    inline const Base::CoordinateTransformation<DIM>* Base::PhysicalElement<DIM>::getTransformation()
    {
        return transform_.get();
    }
    
    template<std::size_t DIM>
    inline void Base::PhysicalElement<DIM>::setPointReference(const Geometry::PointReference<DIM>& point)
    {
        pointReference_ = &point;
        hasPointReference = true;
        //even if they are already computed, the information is now out of date
        hasFunctionValue = false;
        hasVectorFunctionValue = false;
        hasFunctionDeriv = false;
        hasFunctionCurl = false;
        hasFunctionDiv = false;
        hasSolution = false;
        hasVectorSolution = false;
        hasSolutionDeriv = false;
        hasSolutionCurl = false;
        hasSolutionDiv = false;
        hasPointPhysical = false;
        hasJacobian = false;
        hasTransposeJacobian = false;
        hasInverseTransposeJacobian = false;
        hasJacobianDet = false;
        hasJacobianAbsDet = false;
        if(!hasElementMatrix)
        {
            resultMatrix *= 0;
        }
        if(!hasElementVector)
        {
            resultVector *= 0;
        }
        hasElementMatrix = true;
        hasElementVector = true;
    }
    
    template<std::size_t DIM>
    inline void Base::PhysicalElement<DIM>::setElement(const Element* element)
    {
        theElement_ = element;
        if(!hasElement)
        {
            std::size_t numEntries = theElement_->getNumberOfBasisFunctions() * theElement_->getNumberOfUnknowns();
            resultMatrix.resize(numEntries, numEntries);
            resultVector.resize(numEntries);
            basisFunctionValue.resize(theElement_->getNumberOfBasisFunctions());
            vectorBasisFunctionValue.resize(theElement_->getNumberOfBasisFunctions());
            basisFunctionDeriv_.resize(theElement_->getNumberOfBasisFunctions());
            basisFunctionCurl_.resize(theElement_->getNumberOfBasisFunctions());
            basisFunctionDiv_.resize(theElement_->getNumberOfBasisFunctions());
        }
        hasElement = true;
        //even if they are already computed, the information is now out of date
        hasFunctionValue = false;
        hasVectorFunctionValue = false;
        hasFunctionDeriv = false;
        hasFunctionCurl = false;
        hasFunctionDiv = false;
        hasSolution = false;
        hasVectorSolution = false;
        hasSolutionDeriv = false;
        hasSolutionCurl = false;
        hasSolutionDiv = false;
        hasPointPhysical = false;
        hasJacobian = false;
        hasTransposeJacobian = false;
        hasInverseTransposeJacobian = false;
        hasJacobianDet = false;
        hasJacobianAbsDet = false;
        if(!hasElementMatrix)
        {
            resultMatrix *= 0;
        }
        if(!hasElementVector)
        {
            resultVector *= 0;
        }
        hasElementMatrix = true;
        hasElementVector = true;
    }
    
    template<std::size_t DIM>
    inline void Base::PhysicalElement<DIM>::setTransformation(std::shared_ptr<Base::CoordinateTransformation<DIM> >& transform)
    {
        transform_ = transform;
        //even if they are already computed, the information is now out of date
        hasFunctionValue = false;
        hasVectorFunctionValue = false;
        hasFunctionDeriv = false;
        hasFunctionCurl = false;
        hasFunctionDiv = false;
        hasSolution = false;
        hasVectorSolution = false;
        hasSolutionDeriv = false;
        hasSolutionCurl = false;
        hasSolutionDiv = false;
        if(!hasElementMatrix)
        {
            resultMatrix *= 0;
        }
        if(!hasElementVector)
        {
            resultVector *= 0;
        }
        hasElementMatrix = true;
        hasElementVector = true;
    }
    
#endif /* PHYSYCALELEMENT_H_ */

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

#include "SavageHutter.h"

using LinearAlgebra::NumericalVector;

/// \param[in] dimension Dimension of the domain
/// \param[in] numberOfVariables Number of variables in the PDE
/// \param[in] polynomialOrder Polynomial order of the basis functions
/// \param[in] useMatrixStorage Boolean to indicate if element and face matrices for the PDE should be stored
/// \param[in] ptrButcherTableau Pointer to a Butcher Tableau used to do the time integration with a Runge-Kutta scheme. By default this is a RK4 scheme.
SavageHutter::SavageHutter
(
 const std::size_t dimension,
 const std::size_t numOfVariables,
 const std::size_t polynomialOrder,
 const Base::ButcherTableau * const ptrButcherTableau,
 const std::size_t numTimeSteps
 ) :
HpgemAPISimplified(dimension, numOfVariables, polynomialOrder, ptrButcherTableau),
DIM_(dimension), numOfVariables_(numOfVariables), numTimeSteps_(numTimeSteps), timeStepCounter(0)
{
    rhsComputer_.numOfVariables_ = numOfVariables;
    rhsComputer_.DIM_ = dimension;
    rhsComputer_.epsilon_ = 1.0;
    rhsComputer_.theta_ = M_PI / 6; //radians
}

Base::RectangularMeshDescriptor SavageHutter::createMeshDescription(const std::size_t numOfElementPerDirection)
{
    // Create the domain. In this case the domain is the square [0,1]^DIM and periodic.
    Base::RectangularMeshDescriptor description(DIM_);
    for (std::size_t i = 0; i < DIM_; ++i)
    {
        description.bottomLeft_[i] = 0;
        description.topRight_[i] = 1;
        description.numElementsInDIM_[i] = numOfElementPerDirection;
        description.boundaryConditions_[i] = Base::BoundaryType::SOLID_WALL;
    }    
    return description;
}

/// \brief Compute the initial solution at a given point in space and time.
LinearAlgebra::NumericalVector SavageHutter::getInitialSolution(const PointPhysicalT &pPhys, const double &startTime, const std::size_t orderTimeDerivative)
{
    LinearAlgebra::NumericalVector initialSolution(numOfVariables_);
    if (pPhys[0] < M_PI/8)
    {
        initialSolution(0) = 0.6 + 0.5*std::cos(8*pPhys[0]);
    }
    else
    {
        initialSolution(0) = 0.1;
    }
    initialSolution(1) = 2.5 * initialSolution(0);
    return initialSolution;
}

/// \details The integrand for the initial solution is the exact solution at time 0 multiplied by a test function. 
/// The integrand is then scaled by the reference-to-physical element scale, 
/// since we compute the integral on a reference element.
LinearAlgebra::NumericalVector SavageHutter::integrandInitialSolutionOnRefElement
(const Base::Element *ptrElement, const double &startTime, const Geometry::PointReference &pRef)
{
    const std::size_t numOfBasisFunctions = ptrElement->getNrOfBasisFunctions();
    
    LinearAlgebra::NumericalVector integrand(numOfVariables_ * numOfBasisFunctions);
    
    const Geometry::PointPhysical pPhys = ptrElement->referenceToPhysical(pRef);
    
    const LinearAlgebra::NumericalVector initialSolution(getInitialSolution(pPhys, startTime));
    
    std::size_t iVB; // Index for both variable and basis function.
    for (std::size_t iV = 0; iV < numOfVariables_; iV++)
    {
        for (std::size_t iB = 0; iB < numOfBasisFunctions; iB++)
        {
            iVB = ptrElement->convertToSingleIndex(iB, iV);
            integrand(iVB) = ptrElement->basisFunction(iB, pRef) * initialSolution(iV);
        }
    }
    
    // Scale with the reference-to-physical element ratio.
    const Geometry::Jacobian jac = ptrElement->calcJacobian(pRef);
    integrand *= jac.determinant();
    
    return integrand;
}

/*********************Integrate over elements and faces************************/

LinearAlgebra::NumericalVector SavageHutter::integrateInitialSolutionAtElement(Base::Element * ptrElement, const double startTime, const std::size_t orderTimeDerivative)
{
    // Define the integrand function for the the initial solution integral.
    const std::function<LinearAlgebra::NumericalVector(const Geometry::PointReference &)> integrandFunction 
    = [=](const Geometry::PointReference & pRef) -> LinearAlgebra::NumericalVector 
    { 
        return this -> integrandInitialSolutionOnRefElement(ptrElement, startTime, pRef);
    };
    
    const LinearAlgebra::NumericalVector solution = elementIntegrator_.referenceElementIntegral(ptrElement->getGaussQuadratureRule(), integrandFunction);
    return solution;
}

LinearAlgebra::NumericalVector SavageHutter::computeRightHandSideAtElement(Base::Element *ptrElement, LinearAlgebra::NumericalVector &solutionCoefficients, const double time)
{
    // Define the integrand function for the right hand side for the reference element.
    const std::function<LinearAlgebra::NumericalVector(const Base::Element*, const Geometry::PointReference &)> integrandFunction = [=](const Base::Element* elt, const Geometry::PointReference & pRef) -> LinearAlgebra::NumericalVector
    {   
        return rhsComputer_.integrandRightHandSideOnElement(elt, time, pRef, solutionCoefficients);
    };
    
    return elementIntegrator_.integrate(ptrElement, integrandFunction, ptrElement->getGaussQuadratureRule());
}

LinearAlgebra::NumericalVector SavageHutter::computeRightHandSideAtFace
(
 Base::Face *ptrFace,
 const Base::Side side,
 LinearAlgebra::NumericalVector &solutionCoefficientsLeft,
 LinearAlgebra::NumericalVector &solutionCoefficientsRight,
 const double time
 )
{
    const std::function<LinearAlgebra::NumericalVector(const Geometry::PointReference &)> integrandFunction = [=](const Geometry::PointReference & pRef) -> LinearAlgebra::NumericalVector
    {   
        return rhsComputer_.integrandRightHandSideOnRefFace(ptrFace, side, ptrFace->getNormalVector(pRef), pRef, solutionCoefficientsLeft, solutionCoefficientsRight);
    };
    
    return faceIntegrator_.referenceFaceIntegral(ptrFace->getGaussQuadratureRule(), integrandFunction);
}

LinearAlgebra::NumericalVector SavageHutter::computeRightHandSideAtFace
        (
         Base::Face *ptrFace,
         LinearAlgebra::NumericalVector &solutionCoefficients,
         const double time
         )
{
    const std::function<LinearAlgebra::NumericalVector(const Geometry::PointReference &)> integrandFunction = [=](const Geometry::PointReference & pRef) -> LinearAlgebra::NumericalVector
    {   
        return rhsComputer_.integrandRightHandSideOnRefFace(ptrFace, pRef, solutionCoefficients);
    };
    
    return faceIntegrator_.referenceFaceIntegral(ptrFace->getGaussQuadratureRule(), integrandFunction);
}

/******************************Limiting****************************************/
void SavageHutter::computeOneTimeStep(double &time, const double dt)
    {
        std::size_t numOfStages = ptrButcherTableau_->getNumStages();
        
        // Compute intermediate Runge-Kutta stages
        for (std::size_t iStage = 0; iStage < numOfStages; iStage++)
        {
            double stageTime = time + ptrButcherTableau_->getC(iStage) * dt;
            
            std::vector<std::size_t> timeLevelsIn;
            std::vector<double> coefficientsTimeLevels;
            
            timeLevelsIn.push_back(solutionTimeLevel_);
            coefficientsTimeLevels.push_back(1);
            for (std::size_t jStage = 0; jStage < iStage; jStage++)
            {
                timeLevelsIn.push_back(intermediateTimeLevels_[jStage]);
                coefficientsTimeLevels.push_back(dt * ptrButcherTableau_->getA(iStage, jStage));
            }
            
            computeTimeDerivative(timeLevelsIn, coefficientsTimeLevels, intermediateTimeLevels_[iStage], stageTime);
        }
        
        // Update the solution
        for (std::size_t jStage = 0; jStage < numOfStages; jStage++)
        {
            scaleAndAddTimeLevel(solutionTimeLevel_, intermediateTimeLevels_[jStage], dt * ptrButcherTableau_->getB(jStage));
        }
        
        //limitSolution();
        
        // Update the time.
        time += dt;
    }

void SavageHutter::limitSolution()
{
    NumericalVector solutionLeft(2);
    NumericalVector solutionRight(2);
    Geometry::PointReference pRef(0);

    for (Base::Face *face : meshes_[0]->getFacesList())
    {
        if (!face->isInternal())
        {
            continue;
        }
        
        const std::size_t numBasisFuncsLeft = face->getPtrElementLeft()->getNrOfBasisFunctions();
        NumericalVector solutionCoefficientsLeft = face->getPtrElementLeft()->getTimeLevelDataVector(0);
        for (std::size_t i = 0; i < numBasisFuncsLeft; ++i)
        {
            std::size_t iH = face->getPtrElementLeft()->convertToSingleIndex(i, 0);
            solutionLeft(0) += solutionCoefficientsLeft(iH) * face->basisFunction(Base::Side::LEFT, i, pRef);
            std::size_t iHu = face->getPtrElementLeft()->convertToSingleIndex(i, 1);
            solutionLeft(1) += solutionCoefficientsLeft(iHu) * face->basisFunction(Base::Side::LEFT, i, pRef);
        }

        const std::size_t numBasisFuncsRight = face->getPtrElementRight()->getNrOfBasisFunctions();
        NumericalVector solutionCoefficientsRight = face->getPtrElementRight()->getTimeLevelDataVector(0);
        for (std::size_t i = 0; i < numBasisFuncsRight; ++i)
        {
            std::size_t iH = face->getPtrElementRight()->convertToSingleIndex(i, 0);
            solutionRight(0) += solutionCoefficientsRight(iH) * face->basisFunction(Base::Side::RIGHT, i, pRef);
            std::size_t iHu = face->getPtrElementRight()->convertToSingleIndex(i, 1);
            solutionRight(1) += solutionCoefficientsRight(iHu) * face->basisFunction(Base::Side::RIGHT, i, pRef);
        }
    }
}
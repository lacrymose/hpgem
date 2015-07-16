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
#include "SavageHutterRHS2D.h"
#include "HelperFunctions.h"

MiddleSizeVector SavageHutterRHS2D::integrandRightHandSideOnElement
(Base::PhysicalElement<DIM>& element, const double &time, const MiddleSizeVector &solutionCoefficients)
{
    const std::size_t numBasisFuncs = element.getElement()->getNrOfBasisFunctions();
    
    MiddleSizeVector& integrand = element.getResultVector(); //just to have the correct length    
    const PointPhysicalT& pPhys = element.getPointPhysical();
    const PointReferenceT& pRef = element.getPointReference();
    const MiddleSizeVector numericalSolution = Helpers::getSolution<DIM>(element.getElement(), solutionCoefficients, pRef, numOfVariables_);
    logger(DEBUG, "NumericalSolution: %,", numericalSolution);
    const MiddleSizeVector physicalFlux = computePhysicalFlux(numericalSolution);
    const MiddleSizeVector source = computeSourceTerm(numericalSolution, pPhys, time);
    logger.assert(Base::L2Norm(source) < 1e-10, "Source non-zero: %", source);
    
    // Compute integrand on the physical element.
    std::size_t iVB; // Index for both basis function and variable
    for (std::size_t iB = 0; iB < numBasisFuncs; iB++) // Index for basis function
    {
        for (std::size_t iV = 0; iV < numOfVariables_; ++iV)
        {
            iVB = element.getElement()->convertToSingleIndex(iB, iV);
            integrand(iVB) = physicalFlux(2*iV) * element.basisFunctionDeriv(iB)(0);
            integrand(iVB) += physicalFlux(2*iV + 1) * element.basisFunctionDeriv(iB)(1);
            integrand(iVB) += source(iV) * element.basisFunction(iB);
        }
    }
    
    logger(DEBUG, "Integrand on element: %", integrand);
    return integrand;
}

/// \details The integrand for the reference face is the same as the physical face, but scaled with the reference-to-physical face scale. This face scale is absorbed in the normal vector, since it is relatively cheap to compute the normal vector with a length (L2-norm) equal to the reference-to-physical face scale.
MiddleSizeVector SavageHutterRHS2D::integrandRightHandSideOnRefFace
( Base::PhysicalFace<DIM>& face, const Base::Side &iSide, const MiddleSizeVector &solutionCoefficientsLeft, const MiddleSizeVector &solutionCoefficientsRight)
{
    LinearAlgebra::SmallVector<DIM> normal = face.getNormalVector();
    const std::size_t numTestBasisFuncs = face.getFace()->getPtrElement(iSide)->getNrOfBasisFunctions();

    //compute numerical solution at the left side and right side of this face
    const PointReferenceOnFaceT& pRef = face.getPointReference();
    const PointReferenceT& pRefL = face.getFace()->mapRefFaceToRefElemL(pRef);
    const PointReferenceT& pRefR = face.getFace()->mapRefFaceToRefElemR(pRef);
    MiddleSizeVector solutionLeft = Helpers::getSolution<DIM>(face.getFace()->getPtrElementLeft(), solutionCoefficientsLeft, pRefL, numOfVariables_);    
    MiddleSizeVector solutionRight = Helpers::getSolution<DIM>(face.getFace()->getPtrElementRight(), solutionCoefficientsRight, pRefR, numOfVariables_);
    
    logger(DEBUG, "face: %, uL: %, uR:%", face.getFace()->getID(), solutionLeft, solutionRight);
    
    if (iSide == Base::Side::RIGHT) //the normal is defined for the left element
    {
        normal *= -1;
    }
    
    MiddleSizeVector flux = localLaxFriedrichsFlux(solutionLeft, solutionRight, normal);
    
    if (face.getFace()->getID() == 14)
    {
        logger(DEBUG, "flux on face %: %",face.getFace()->getID(), flux);
        logger(DEBUG, "numerical solutions on face %: %, %", face.getFace()->getID(), solutionLeft, solutionRight);
        logger(DEBUG, "normal on face %: %", face.getFace()->getID(), normal);
    }
    
    MiddleSizeVector& integrand = face.getResultVector(iSide); // Integrand value based on n number of testbasisfunctions from element corresponding to side iSide

    for (std::size_t iFun = 0; iFun < numTestBasisFuncs; ++iFun)
    {
        for (std::size_t iVar = 0; iVar < numOfVariables_; ++iVar)
        {
            std::size_t iVarFun = face.getFace()->getPtrElement(iSide)->convertToSingleIndex(iFun, iVar);
            integrand(iVarFun) = -flux(iVar) * face.basisFunction(iSide, iFun);
        }
    }
    logger(DEBUG, "integrand on internal face %: %", face.getFace()->getID(), integrand);
    return integrand;
}

MiddleSizeVector SavageHutterRHS2D::integrandRightHandSideOnRefFace
    (
     Base::PhysicalFace<DIM>& face,
     const MiddleSizeVector &solutionCoefficients
     )
{
    double normalX = face.getNormalVector()[0];
    const std::size_t numBasisFuncs = face.getFace()->getNrOfBasisFunctions();
    
    const PointReferenceOnFaceT& pRef = face.getPointReference();
    //note that at the boundary, the element is the left element by definition
    const PointReferenceT& pRefL = face.getFace()->mapRefFaceToRefElemL(pRef);
    MiddleSizeVector solution = Helpers::getSolution<DIM>(face.getFace()->getPtrElementLeft(), solutionCoefficients, pRefL, numOfVariables_);
    
    MiddleSizeVector flux(3);
    
    //outflow
    if (normalX > 0)
    {
        flux = localLaxFriedrichsFlux(solution, solution, face.getNormalVector());
    }
    else //inflow
    {
        flux = localLaxFriedrichsFlux(solution, inflowBC_, face.getNormalVector());
    }
    
    MiddleSizeVector integrand(numOfVariables_ * numBasisFuncs);

    for (std::size_t iFun = 0; iFun < numBasisFuncs; ++iFun)
    {
        for (std::size_t iVar = 0; iVar < numOfVariables_; ++iVar)
        {
            std::size_t iVarFun = face.getFace()->getPtrElementLeft()->convertToSingleIndex(iFun, iVar);
            integrand(iVarFun) = -flux(iVar) * face.basisFunction(iFun);
        }
    }
    
    return integrand;
}

MiddleSizeVector SavageHutterRHS2D::computePhysicalFlux(const MiddleSizeVector &numericalSolution)
{    
    const double h = numericalSolution(0);
    logger.assert(h > -1e-16, "Negative height (%)", h);
    double hu = numericalSolution(1);
    double hv = numericalSolution(2);
    double u = 0;
    double v = 0;
    if (h > minH_)
    {
        u = hu / h;
        v = hv / h;
    }
    MiddleSizeVector flux(6);
    flux(0) = hu;
    flux(1) = hv;
    flux(2) = hu * u + epsilon_/2 * std::cos(chuteAngle_) * h * h;
    flux(3) = hu*v;
    flux(4) = hu*v;
    flux(5) = hv * v + epsilon_/2 * std::cos(chuteAngle_) * h * h;
    logger(DEBUG, "flux values: %, ", flux);
    return flux;
}

MiddleSizeVector SavageHutterRHS2D::computeSourceTerm(const MiddleSizeVector& numericalSolution, const PointPhysicalT& pPhys, const double time)
{
    logger.assert(chuteAngle_ < M_PI / 2, "Angle must be in radians, not degrees!");
    const double h = numericalSolution(0);
    const double hu = numericalSolution(1);
    double u = 0;
    if (h > minH_)
    {
        u = hu/h;
    }
    double mu = computeFriction(numericalSolution);
    const int signU = Helpers::sign(u);
    double sourceX = h * std::sin(chuteAngle_) - h * mu * signU * std::cos(chuteAngle_);
    logger(DEBUG, "Source: %, h: %", sourceX, h);
    return MiddleSizeVector({0, sourceX, 0});
}

MiddleSizeVector SavageHutterRHS2D::localLaxFriedrichsFlux(const MiddleSizeVector& numericalSolutionLeft, const MiddleSizeVector& numericalSolutionRight,const LinearAlgebra::SmallVector<DIM>& normal)
{
    double uLeft = 0;
    double vLeft = 0;
    if (numericalSolutionLeft(0) > minH_)
    {
        uLeft = numericalSolutionLeft(1) / numericalSolutionLeft(0);
        vLeft = numericalSolutionLeft(2) / numericalSolutionLeft(0);
    }
    
    double uRight = 0;
    double vRight = 0;
    if (numericalSolutionRight(0) > minH_)
    {
        uRight = numericalSolutionRight(1) / numericalSolutionRight(0);
        vRight = numericalSolutionRight(2) / numericalSolutionRight(0);
    }
    
    const double alpha = std::max(std::max(std::abs(uLeft), std::abs(vLeft)) + std::sqrt(epsilon_ * std::max(0.,numericalSolutionLeft(0))), 
                      std::max(std::abs(uRight), std::abs(vRight)) + std::sqrt(epsilon_ * std::max(0.,numericalSolutionRight(0))));
    
    logger(DEBUG, "alpha: %", alpha);
        
    MiddleSizeVector fluxLeft = computePhysicalFlux(numericalSolutionLeft);
    MiddleSizeVector fluxNormalLeft(numOfVariables_);
    MiddleSizeVector fluxRight = computePhysicalFlux(numericalSolutionRight);
    MiddleSizeVector fluxNormalRight(numOfVariables_);
    logger(DEBUG, "physical fluxes: %, %", fluxLeft, fluxRight);
    for (std::size_t i = 0; i < numOfVariables_; ++i)
    {
        fluxNormalLeft(i) = fluxLeft(2*i)*normal(0) + fluxLeft(2*i+1) * normal(1);
        fluxNormalRight(i) = fluxRight(2*i)*normal(0) + fluxRight(2*i+1) * normal(1);
    }
    
    logger(DEBUG, "normal times physical fluxes: %, %", fluxNormalLeft, fluxNormalRight);
    MiddleSizeVector diffSolutions = numericalSolutionRight - numericalSolutionLeft;
    diffSolutions = diffSolutions * normal(0) + diffSolutions * normal(1);
    
    const MiddleSizeVector numericalFlux = 0.5 * 
        (fluxNormalLeft + fluxNormalRight - alpha * (diffSolutions));
    
    return numericalFlux;
}

double SavageHutterRHS2D::computeFriction(const MiddleSizeVector& numericalSolution)
{
    return std::tan(chuteAngle_);
}
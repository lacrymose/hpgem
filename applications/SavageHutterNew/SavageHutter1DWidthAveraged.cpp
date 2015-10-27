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

#include "SavageHutter1DWidthAveraged.h"
#include "HelperFunctions.h"
#include "SlopeLimiters/TvbLimiterWithDetector1D.h"
#include "SlopeLimiters/TvbLimiter1D.h"

///\details In this constructor, some of the parameters for the problem are set.
///Most of these parameters are declared in SavageHutterBase, but since they are protected
///they can also be used here.
SavageHutter1DWidthAveraged::SavageHutter1DWidthAveraged(std::size_t polyOrder, std::size_t numberOfElements)
: SavageHutter1DBase(2, polyOrder)
{
    chuteAngle_ = M_PI / 180 * 30;
    epsilon_ = .1;
    const PointPhysicalT &pPhys = createMeshDescription(1).bottomLeft_;
    inflowBC_ = getInitialSolution(pPhys, 0);
    
    std::vector<std::string> variableNames = {"hW", "huW"};
    setOutputNames("output1D", "SavageHutter", "SavageHutter", variableNames);
    
    createMesh(numberOfElements, Base::MeshType::RECTANGULAR);
    
}

///\details In this function, the mesh gets described. 
///The domain is given by [0, endOfDomain], it consists of numberOfElementsPerDirection
///elements (in this case lines) and has either periodic or non-periodic boundary
///conditions. If the boundary conditions are not periodic, set the BoundaryType
///in this function to Base::BoundaryType::SOLID_WALL and make sure that the function
///SavageHutter1DBase::integrandRightHandSideOnRefFace is set correctly for boundary
///faces. The ghost solution on the boundary can be described with computeGhostSolution.
Base::RectangularMeshDescriptor<1> SavageHutter1DWidthAveraged::createMeshDescription(const std::size_t numOfElementsPerDirection)
{
    const double endOfDomain = 1;
    const Base::BoundaryType boundary = Base::BoundaryType::SOLID_WALL;
    return SavageHutter1DBase::createMeshDescription(numOfElementsPerDirection, endOfDomain, boundary);
}

///\details Gives the initial solution for the problem. One could also call getExactSolution
///here to get the analytical solution at the start time.
 LinearAlgebra::MiddleSizeVector SavageHutter1DWidthAveraged::getInitialSolution(const PointPhysicalT &pPhys, const double &startTime, const std::size_t orderTimeDerivative)
{
    const double x = pPhys[0];
    const double width = getWidth(pPhys)[0];
    double h = 1;
    const double hu = 2;
    if (x > 0.5)
    {
        h = 1;
    }
    return LinearAlgebra::MiddleSizeVector({h*width, hu*width});
}

 ///\details Gives the exact (analytical) solution for the test problem. This function
 ///is not necessary, unless the last flag in main::solve has been set to true.
LinearAlgebra::MiddleSizeVector SavageHutter1DWidthAveraged::getExactSolution(const PointPhysicalT& pPhys, const double& time, const std::size_t orderTimeDerivative)
{
    double h = 0;
    double hu = 0;
    const double width = getWidth(pPhys)[0];
    if (pPhys[0] > 0.5)
    {
        h = 0;
        hu = 0;
    }
    return LinearAlgebra::MiddleSizeVector({h*width, hu*width});
}

///\details Constructs the slope limiter, available slope limiters can be found 
///in the folder SlopeLimiters.
SlopeLimiter* SavageHutter1DWidthAveraged::createSlopeLimiter()
{
    return new EmptySlopeLimiter;
    //Little hack: the polynomial order is the number of basis functions minus one.
    //return new TvbLimiterWithDetector1D(numberOfVariables_, inflowBC_, (*meshes_[0]->getElementsList().begin())->getNumberOfBasisFunctions() - 1);
    //return new TvbLimiter1D(numberOfVariables_);
}

///\details Constructs the non-negativity limiter, available non-negativity limiters
///can be found in the folder HeightLimiters.
HeightLimiter* SavageHutter1DWidthAveraged::createHeightLimiter()
{
    return new EmptyHeightLimiter;
}

///\details Write the values of the variables to the VTK file. The method of 
///HpgemAPISimplified writes all the unknowns of the system (here hW and huW).
void SavageHutter1DWidthAveraged::registerVTKWriteFunctions()
{
    HpgemAPISimplified::registerVTKWriteFunctions();
}

///\details Compute the source term of the 1D shallow granular flow system, namely
///h(\sin\theta - \mu\sign(u)\cos\theta). It is important to note here that one can
///choose between a Coulomb-type friction law, a Pouliquen friction law as can be found
///in Weinhart et. al. (2012) or an exponential form for the friction. The respective functions
///are defined in the class SavageHutter1DBase.
LinearAlgebra::MiddleSizeVector SavageHutter1DWidthAveraged::computeSourceTerm(const LinearAlgebra::MiddleSizeVector& numericalSolution, const PointPhysicalT& pPhys, const double time)
{
    logger.assert(chuteAngle_ < M_PI, "Angle must be in radians, not degrees!");    
    const double width = getWidth(pPhys)[0];
    const double widthChange = getWidth(pPhys)[1];
    const double h = numericalSolution(0) / width;
    const double hu = numericalSolution(1) / width;
    double u = 0;
    if (h > dryLimit_)
    {
        u = hu / h;
    }
    double mu = computeFrictionCoulomb(numericalSolution, 20./180*M_PI);
    const int signU = Helpers::sign(u);
    double sourceX = h * width * (std::sin(chuteAngle_) - mu * signU * std::cos(chuteAngle_)) + epsilon_*std::cos(chuteAngle_)*h*h*widthChange;
    logger(DEBUG, "Source: %, h: %", sourceX, h);
    return MiddleSizeVector({0, sourceX});
}

///\details Compute the function f(h,hu) = {hu, \alpha hu^2 + h^2/2 \epsilon \cos \theta}
LinearAlgebra::MiddleSizeVector SavageHutter1DWidthAveraged::computePhysicalFlux(const LinearAlgebra::MiddleSizeVector &numericalSolution, const PointPhysicalT &pPhys)
{
    const double width = getWidth(pPhys)[0];
    const double hW = numericalSolution(0);
    const double h = hW / width;
    logger.assert_always(h > -1e-16, "Negative height (%)", h);
    const double hu = numericalSolution(1) / width;
    double u = 0;
    if (h > dryLimit_)
    {
        u = hu / h;
    }
    MiddleSizeVector flux(2);
    flux(0) = hu;
    flux(1) = hu * u * width + epsilon_ / 2 * std::cos(chuteAngle_) * h * h * width;
    logger(DEBUG, "flux values: %, %", flux(0), flux(1));
    return flux;
}

///\brief Define your boundary conditions here
LinearAlgebra::MiddleSizeVector SavageHutter1DWidthAveraged::computeGhostSolution(const LinearAlgebra::MiddleSizeVector &solution, const double normal, const double time)
{
   const double h = solution[0];
    const double u = h > dryLimit_ ? solution[1] / h : 0;
    const double froude = std::abs(u) / std::sqrt(epsilon_ * std::cos(chuteAngle_) * h);
    if (normal < 0) //inflow boundary
    {
        if (true || froude >= 1)
        {
            return inflowBC_;
        }
        else
        {
            const double hOut = inflowBC_[0];
            const double uOut = u - 2 * std::sqrt(epsilon_ * std::cos(chuteAngle_))*(std::sqrt(h) - std::sqrt(hOut));
            logger(INFO, "h and u: %, %", hOut, uOut);
            return MiddleSizeVector({hOut, hOut * uOut});
        }
    }
    else //outflow boundary
    {
        if (true || froude >= 1)
        {
            return solution;
        }
        else
        {
            double invariantIn = u + 2 * std::sqrt(epsilon_ * std::cos(chuteAngle_) * h);
            double froudePrescribed = 1;
            //double hOut = (invariantIn / (2 + froudePrescribed)) * (invariantIn / (2 + froudePrescribed)) / (epsilon_ * std::cos(chuteAngle_));
            double hOut = h;
            double uOut = invariantIn - 2 * std::sqrt(epsilon_ * std::cos(chuteAngle_) * hOut);
            logger(DEBUG, "new h and u and F: %, %, %", hOut, uOut, uOut / std::sqrt(epsilon_ * std::cos(chuteAngle_) * hOut));
            return MiddleSizeVector({hOut, hOut * uOut});
        }
    }
}

std::array<double, 2> SavageHutter1DWidthAveraged::getWidth(const PointPhysicalT &pPhys) const
{
    double width = 1;
    double widthDerivX = 0;
    return {width, widthDerivX};
}

/// Compute friction as described in Weinhart et al (2012), eq (50)
/// Notice that the minus sign for gamma in eq (50) is wrong.
double SavageHutter1DWidthAveraged::computeFriction(const double h, const double u)
{
    const double delta1 = 17.518 / 180 * M_PI;
    const double delta2 = 29.712 / 180 * M_PI;
    const double A = 5.29;
    const double beta = 0.189;
    const double gamma = -.080;
    const double d = 2;
    if (h < dryLimit_)
        return std::tan(delta1);
    const double F = u / std::sqrt(epsilon_ * std::cos(chuteAngle_) * h);
    return std::tan(delta1) + (std::tan(delta2) - std::tan(delta1)) / (beta * h / (A * d * (F - gamma)) + 1);
}

LinearAlgebra::MiddleSizeVector SavageHutter1DWidthAveraged::hllcFlux(const LinearAlgebra::MiddleSizeVector& numericalSolutionLeft, const LinearAlgebra::MiddleSizeVector& numericalSolutionRight, const double normal, Base::PhysicalFace<1> &face)
{
    const double width = getWidth(face.getPointPhysical())[0];
    const double hLeft = numericalSolutionLeft[0]/width;
    const double hRight = numericalSolutionRight[0]/width;
    double normalSpeedLeft = 0;
    if (numericalSolutionLeft(0) > dryLimit_)
    {
        normalSpeedLeft = normal * numericalSolutionLeft(1) / numericalSolutionLeft(0);
    }

    double normalSpeedRight = 0;
    if (numericalSolutionRight(0) > dryLimit_)
    {
        normalSpeedRight = normal * numericalSolutionRight(1) / numericalSolutionRight(0);
    }
    LinearAlgebra::MiddleSizeVector fluxNormalLeft = normal * computePhysicalFlux(numericalSolutionLeft, face.getPointPhysical());
    LinearAlgebra::MiddleSizeVector fluxNormalRight = normal * computePhysicalFlux(numericalSolutionRight, face.getPointPhysical());
    double phaseSpeedLeft = std::sqrt(epsilon_ * std::cos(chuteAngle_) * hLeft);
    double phaseSpeedRight = std::sqrt(epsilon_ * std::cos(chuteAngle_) * hRight);
    double sl = std::min(normalSpeedLeft - phaseSpeedLeft, normalSpeedRight - phaseSpeedRight);
    double sr = std::max(normalSpeedLeft + phaseSpeedLeft, normalSpeedRight + phaseSpeedRight);
    if (sl >= 0)
    {
        return fluxNormalLeft;
    }
    if (sr <= 0)
    {
        return fluxNormalRight;
    }
    return ((sr * fluxNormalLeft - sl * fluxNormalRight + sl * sr * (numericalSolutionRight - numericalSolutionLeft)) / (sr - sl));
}
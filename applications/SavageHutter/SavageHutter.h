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

#ifndef SavageHutterH
#define SavageHutterH

#include <fstream>
#include <iomanip> 

#include "HelperFunctions.h"

#include "Base/CommandLineOptions.h"
#include "Base/ConfigurationData.h"
#include "Base/Element.h"
#include "Base/Face.h"
#include "Base/HpgemAPISimplified.h"
#include "Base/MpiContainer.h"
#include "Base/RectangularMeshDescriptor.h"
#include "Base/TimeIntegration/AllTimeIntegrators.h"
#include "Geometry/PointReference.h"
#include "Geometry/PointPhysical.h"
#include "Integration/ElementIntegral.h"
#include "Integration/FaceIntegral.h"
#include "Output/TecplotDiscontinuousSolutionWriter.h"
#include "Output/TecplotSingleElementWriter.h"
#include "SavageHutterRightHandSideComputer.h"

#include "Logger.h"
#include "SlopeLimiter.h"
#include "PositiveLayerLimiter.h"


/// \param[in] numberOfVariables Number of variables in the PDE
/// \param[in] polynomialOrder Polynomial order of the basis functions
/// \param[in] useMatrixStorage Boolean to indicate if element and face matrices for the PDE should be stored
/// \param[in] ptrButcherTableau Pointer to a Butcher Tableau used to do the time integration with a Runge-Kutta scheme. By default this is a RK4 scheme.
struct SHConstructorStruct
{
    std::size_t numOfVariables;
    std::size_t polyOrder;
    std::size_t numElements;
    Base::MeshType meshType;
    Base::ButcherTableau * ptrButcherTableau;
};


//todo: make the functions override final, but at the moment my parser does not 
//understand the override and final keywords, which makes development harder
class SavageHutter : public Base::HpgemAPISimplified<DIM>
{
public:
    
    ///Alternative constructor with less input parameters. Furthermore, this 
    ///constructor also constructs the mesh and couples an object LimiterData to
    ///each element.
    SavageHutter(const SHConstructorStruct& inputValues);
    
    ~SavageHutter()
    {
        //delete rhscomputer, slope limiter and non-negativity limiter
    }

    /// \brief Create a domain
    Base::RectangularMeshDescriptor<DIM> createMeshDescription(const std::size_t numOfElementPerDirection);

    /// \brief Compute the initial solution at a given point in space and time.
    LinearAlgebra::MiddleSizeVector getInitialSolution(const PointPhysicalT &pPhys, const double &startTime, const std::size_t orderTimeDerivative = 0);
    
    /// \brief Show the progress of the time integration.
    void showProgress(const double time, const std::size_t timeStepID)
    {
        if (timeStepID % 100 == 0)
        {
            logger(INFO, "% time steps computed.", timeStepID);
        }
    }

    LinearAlgebra::MiddleSizeVector computeRightHandSideAtElement(Base::Element *ptrElement, LinearAlgebra::MiddleSizeVector &solutionCoefficients, const double time);

    /// \brief Compute the right-hand side corresponding to an internal face
    LinearAlgebra::MiddleSizeVector computeRightHandSideAtFace(Base::Face *ptrFace,
            const Base::Side side,
            LinearAlgebra::MiddleSizeVector &solutionCoefficientsLeft,
            LinearAlgebra::MiddleSizeVector &solutionCoefficientsRight,
            const double time);
    
    LinearAlgebra::MiddleSizeVector computeRightHandSideAtFace
        (
         Base::Face *ptrFace,
         LinearAlgebra::MiddleSizeVector &solutionCoefficients,
         const double time
         );
    
    void computeOneTimeStep(double &time, const double dt);
    void limitSolution();
    
    ///Compute the minimum of the height in the given element
    double getMinimumHeight(const Base::Element *element);
    
    void tasksBeforeSolving() override final
    {
        //todo: for one face integral you used referenceFaceIntegral (which does not scale with the magnitude of the normal) and for the other you used integrate (which does scale)
        //so it is not clear to me whether or not you need scaling. Please fix as needed
        this->faceIntegrator_.setTransformation(std::shared_ptr<Base::CoordinateTransformation<DIM> >(new Base::DoNotScaleIntegrands<DIM>(new Base::H1ConformingTransformation<DIM>())));
        Base::HpgemAPISimplified<DIM>::tasksBeforeSolving();
    }
    
    void initialDryWet()
    {
        for (Base::Element *elt : meshes_[0]->getElementsList())
        {
            logger(DEBUG, "coefficients: %", elt->getTimeLevelData(0));
            Helpers::DryFlag* flagStruct = static_cast<Helpers::DryFlag*>(elt->getUserData());
            if (Helpers::computeAverageOfSolution<DIM>(elt, elementIntegrator_) (0) < minH_)
            {
                flagStruct->isDry = true;
            }
            else
            {
                flagStruct->isDry = false;
            }
        }
    }
    
private:

    /// Number of variables
    const std::size_t numOfVariables_;

    RightHandSideComputer* rhsComputer_;
    
    SlopeLimiter* slopeLimiter_;
    
    HeightLimiter* heightLimiter_;
    
    const double minH_;
    
    LinearAlgebra::MiddleSizeVector inflowBC_;
    
};

#endif

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

#ifndef AcousticWaveH
#define AcousticWaveH

#include <fstream>

#include "Base/CommandLineOptions.h"
#include "Base/ConfigurationData.h"
#include "Base/Element.h"
#include "Base/Face.h"
#include "Base/HpgemAPISimplified.h"
#include "Base/MpiContainer.h"
#include "Base/RectangularMeshDescriptor.h"
#include "Base/TimeIntegration/AllTimeIntegrators.h"
#include "Geometry/PointReference.h"
#include "Integration/ElementIntegral.h"
#include "Integration/FaceIntegral.h"
#include "Output/TecplotDiscontinuousSolutionWriter.h"
#include "Output/TecplotSingleElementWriter.h"
#include "Utilities/BasisFunctions1DH1ConformingLine.h"
#include "Utilities/BasisFunctions2DH1ConformingSquare.h"
#include "Utilities/BasisFunctions2DH1ConformingTriangle.h"
#include "Utilities/BasisFunctions3DH1ConformingCube.h"
#include "Utilities/BasisFunctions3DH1ConformingTetrahedron.h"

#include "Logger.h"

/// \brief Class to demonstrate how a system of multiple PDE's can be solved, using the interface HpgemAPISimplified.
/** \details
 
 This class can solve the scalar wave equation in 2D or 3D written as a first order system of PDE's. The original scalar wave equation is given by \f[ \partial_t^2 u = \nabla \cdot c \nabla u \f], where \f$u \f$ is the scalar variable and \f$c \f$ a material parameter corresponding to the velocity with which waves can propagate.
 
 For a first order scheme we define the scalar function \f$ v := \partial_t u \f$ and the vector function \f$ s := c \nabla u \f$. We can then obtain the equations \f[ \partial_t v = \nabla \cdot s \f] and \f[ c^{-1} \partial_t s = \nabla v \f]
 
 We define a new vector function \f$w = [w_0, w_1, w_2, ..] = [v, s_0, s_1, ..]\f$. We can then rewrite the system of PDE's as follows: \f[ \partial_t w_0 = \partial_i w_{i+1} \f] summing over i = 0 .. (DIM-1) and \f[ c^{-1} \partial_t w_{i+1} = \partial_i w_0 \f] for i = 0 .. (DIM-1).
 
 We use periodic boundary conditions or free surface conditions (\f$ \hat{n}\cdot s=0 \f$, with \f$ \hat{n} \f$ the normal unit vector).
 
 This class consists of the following parts:
 \li A constructor to set the dimension, number of elements, polynomial order, butcher tableau, and boolean for storing matrices.
 \li The functions 'createMesh' and 'setMaterialParameter' are used to create the mesh and set the material parameters.
 \li The functions 'getCInv', 'getSourceTerm' and 'getExactSolution' return the material parameters, source term and analytic solution.
 \li The functions 'integrand...OnRefElement' and 'integrand...OnRefElement' compute the integrand for ... for the reference element/face. These functions are necessary to compute the mass matrix, right-hand side, initial solution and numerical error.
 \li The function 'solve' solves the PDE over the time interval [startTime, endTime].
 \li The function 'writeToTecplotFile' is used to determine which data should be written to the output file.
 */
/** \details To solve the problem, the following things are done in the main routine:
 \li An object of the class AcousticWave is created.
 \li The mesh is created with 'createMesh'.
 \li The material parameters are set using 'setMaterialParameters'.
 \li The names for the output files are set using 'setOutputNames'.
 \li The function 'solve' is then used to solve the PDE.
 */

class AcousticWave : public Base::HpgemAPISimplified
{
public:
    AcousticWave
    (
     const std::size_t dimension,
     const std::size_t numOfVariables,
     const std::size_t polynomialOrder,
     const Base::ButcherTableau * const ptrButcherTableau
     );
        
    /// \brief Create a domain
    Base::RectangularMeshDescriptor createMeshDescription(const std::size_t numOfElementPerDirection) override final;

    /// \brief Set the material parameter.
    /// \param[in] c Material parameter corresponding to the speed with which waves can propagate.
    void setMaterialParameter(const double c)
    {
        cInv_ = 1.0 / c;
    }

    /// \brief Get the material parameter c^{-1} at a given physical point.
    double getCInv(const Geometry::PointPhysical &pPhys)
    {
        return cInv_;
    }

    /// \brief Compute the real solution at a given point in space and time.
    LinearAlgebra::NumericalVector getExactSolution(const PointPhysicalT &pPhys, const double &time, const std::size_t orderTimeDerivative = 0) override final;

    /// \brief Compute the initial solution at a given point in space and time.
    LinearAlgebra::NumericalVector getInitialSolution(const PointPhysicalT &pPhys, const double &startTime, const std::size_t orderTimeDerivative = 0) override final;

    /// \brief Compute the integrand for the mass matrix for the reference element.
    LinearAlgebra::Matrix integrandMassMatrixOnRefElement(const Base::Element *ptrElement, const Geometry::PointReference &pRef);

    /// \brief Compute the integrand for the reference element for obtaining the initial solution.
    LinearAlgebra::NumericalVector integrandInitialSolutionOnRefElement(const Base::Element *ptrElement, const double &startTime, const Geometry::PointReference &pRef);

    /// \brief Compute the integrand for the right hand side for the reference element.
    LinearAlgebra::NumericalVector integrandRightHandSideOnRefElement
    (
     const Base::Element *ptrElement,
     const double &time,
     const Geometry::PointReference &pRef,
     const LinearAlgebra::NumericalVector &solutionCoefficients
     );
    
    /// \brief Compute the integrand for the right hand side for the reference face corresponding to a boundary face.
    LinearAlgebra::NumericalVector integrandRightHandSideOnRefFace
    (
     const Base::Face *ptrFace,
     const double &time,
     const Geometry::PointReference &pRef,
     const LinearAlgebra::NumericalVector &solutionCoefficients
     );

    /// \brief Compute the integrand for the right hand side for the reference face corresponding to an internal face.
    LinearAlgebra::NumericalVector integrandRightHandSideOnRefFace
    (
     const Base::Face *ptrFace,
     const double &time,
     const Geometry::PointReference &pRef,
     const Base::Side &iSide,
     const LinearAlgebra::NumericalVector &solutionCoefficientsLeft,
     const LinearAlgebra::NumericalVector &solutionCoefficientsRight
     );

    /// \brief Compute the integrand for the reference element for computing the energy-norm of the error.
    LinearAlgebra::NumericalVector integrandErrorOnRefElement
    (
     const Base::Element *ptrElement,
     const double &time,
     const Geometry::PointReference &pRef,
     const LinearAlgebra::NumericalVector &solutionCoefficients
     );

    /// \brief Compute the mass matrix for a single element.
    LinearAlgebra::Matrix computeMassMatrixAtElement(Base::Element *ptrElement) override final;

    /*
    /// \brief Solve the mass matrix equations for a single element.
    void solveMassMatrixEquationsAtElement(Base::Element *ptrElement, LinearAlgebra::NumericalVector &solutionCoefficients) override;
     */

    /// \brief Integrate the initial solution for a single element.
    LinearAlgebra::NumericalVector integrateInitialSolutionAtElement(Base::Element * ptrElement, const double startTime, const std::size_t orderTimeDerivative) override final;

    /// \brief Integrate the energy of the error on a single element.
    LinearAlgebra::NumericalVector integrateErrorAtElement(Base::Element *ptrElement, LinearAlgebra::NumericalVector &solutionCoefficients, double time) override final;

    /// \brief Compute the right-hand side corresponding to an element
    LinearAlgebra::NumericalVector computeRightHandSideAtElement(Base::Element *ptrElement, LinearAlgebra::NumericalVector &solutionCoefficients, const double time) override final;
    
    /// \brief Compute the right-hand side corresponding to a boundary face
    LinearAlgebra::NumericalVector computeRightHandSideAtFace
    (
     Base::Face *ptrFace,
     LinearAlgebra::NumericalVector &solutionCoefficients,
     const double time
     ) override final;
    
    /// \brief Compute the right-hand side corresponding to an internal face
    LinearAlgebra::NumericalVector computeRightHandSideAtFace
    (
     Base::Face *ptrFace,
     const Base::Side side,
     LinearAlgebra::NumericalVector &solutionCoefficientsLeft,
     LinearAlgebra::NumericalVector &solutionCoefficientsRight,
     const double time
     ) override final;
    
    /// \brief Show the progress of the time integration.
    void showProgress(const double time, const std::size_t timeStepID) override final
    {
        if (timeStepID % 10 == 0)
        {
            logger(INFO, "% time steps computed.", timeStepID);
        }
    }

private:
    /// Dimension of the domain
    const std::size_t DIM_;

    /// Number of variables
    const std::size_t numOfVariables_;

    /// Material parameter c^{-1}
    double cInv_;
};

#endif
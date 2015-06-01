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

#include "CompressibleNavierStokes.h"
#include "Viscous.h"

Viscous::Viscous(const CompressibleNavierStokes& instance) : instance_(instance), PrInv_(1/0.71), cp_(1000), ATensorInternal_(instance_.DIM_*instance_.DIM_), ATensorExternal_(instance_.DIM_*instance_.DIM_)
{
}

double Viscous::computeTemperature(const LinearAlgebra::NumericalVector qSolution, const double pressure)
{
	return pressure/qSolution(0)*instance_.gamma_/cp_; //T = p/(R*rho);
}

void Viscous::setInverseStabilityMassMatrix(LinearAlgebra::Matrix &stabilityMassMatrix)
{
	stabilityMassMatrix_ = stabilityMassMatrix;
}

double Viscous::computeViscosity(double temperature)
{
	double temperatureS =  110;
	double temperatureRef = 288.16;
	double muRef = 0.000017894;

	double temp = temperature/temperatureRef;

	return muRef*(temp)*sqrt(temp)*(temperatureRef + temperatureS)/(temperature + temperatureS);
}


void Viscous::computeStabilityFluxFunction(const std::vector<LinearAlgebra::Matrix> &ATensorInternal, const std::vector<LinearAlgebra::Matrix> &ATensorExternal, const LinearAlgebra::NumericalVector &stateInternal, const LinearAlgebra::NumericalVector &stateExternal, const LinearAlgebra::NumericalVector &normalInternal)
{
	//Compute velocity normal matrix
	LinearAlgebra::Matrix velocityNormal(instance_.DIM_+2,instance_.DIM_);
	LinearAlgebra::NumericalVector qSolutionDifference;
	LinearAlgebra::NumericalVector stateDifference = stateInternal - stateExternal;
	Base::Side eSide;
	for (std::size_t iV = 0; iV < instance_.numOfVariables_; iV++)
	{
		for (std::size_t iD = 0; iD < instance_.DIM_; iD++)
		{
			velocityNormal(iV,iD) = 0.5*stateDifference(iV)*normalInternal(iD);
		}
	}

	//Compute fluxFunction
	stabilityFluxFunctionInternal_ = computeATensorMatrixContraction(ATensorInternal, velocityNormal);
	stabilityFluxFunctionExternal_ = computeATensorMatrixContraction(ATensorExternal, velocityNormal);
}

LinearAlgebra::NumericalVector Viscous::integrandStabilityRightHandSideOnRefFace(
		const Base::Face *ptrFace,
		const Base::Side side,
		const LinearAlgebra::Matrix stabilityFluxFunction,
		const std::size_t iD,
		const Geometry::PointReference &pRef)
{
	std::size_t numBasisFunctions = ptrFace->getPtrElement(side)->getNrOfBasisFunctions();
	std::size_t pos;
	LinearAlgebra::NumericalVector integrand(numBasisFunctions*instance_.numOfVariables_);

	for (std::size_t iE = 0; iE < instance_.numOfVariables_; iE++)
	{
		for (std::size_t iB = 0; iB < numBasisFunctions; iB++)
		{
			pos = numBasisFunctions*iE +iB;
			integrand(pos) = stabilityFluxFunction(iE,iD)*ptrFace->basisFunction(side, iB, pRef);
		}
	}

	double area = Base::L2Norm(ptrFace->getNormalVector(pRef));
	return integrand*area;
}

LinearAlgebra::NumericalVector Viscous::computeRhs(const Base::Face *ptrFace, const Base::Side side, const LinearAlgebra::Matrix stabilityFluxFunction, const std::size_t iD)
{
	//Integrate the required rhs
    std::function<LinearAlgebra::NumericalVector(const Geometry::PointReference &)> integrandFunction = [=](const Geometry::PointReference &pRef) -> LinearAlgebra::NumericalVector
    {   return this->integrandStabilityRightHandSideOnRefFace(ptrFace, side, stabilityFluxFunction, iD, pRef);};
	return instance_.faceIntegrator_.referenceFaceIntegral(ptrFace->getGaussQuadratureRule(), integrandFunction);
}

LinearAlgebra::Matrix Viscous::computeStabilityParameters(
		const Base::Face *ptrFace, Base::Side iSide,
		const std::vector<LinearAlgebra::Matrix> ATensorInternal,
		const std::vector<LinearAlgebra::Matrix> ATensorExternal,
		const LinearAlgebra::NumericalVector stateInternal,
		const LinearAlgebra::NumericalVector stateExternal,
		const LinearAlgebra::NumericalVector normalInternal,
		const Geometry::PointReference &pRef)
{
	Base::Side eSide;
	std::size_t numOfTestBasisFunctionsInternal;
	std::size_t numOfTestBasisFunctionsExternal;

	if (iSide == Base::Side::RIGHT)
	{
		eSide = Base::Side::LEFT;
		numOfTestBasisFunctionsInternal = ptrFace->getPtrElement(iSide)->getNrOfBasisFunctions();
		numOfTestBasisFunctionsExternal= ptrFace->getPtrElement(eSide)->getNrOfBasisFunctions();
	}
	else
	{
		eSide = Base::Side::RIGHT;
		numOfTestBasisFunctionsInternal = ptrFace->getPtrElement(iSide)->getNrOfBasisFunctions();
		numOfTestBasisFunctionsExternal = ptrFace->getPtrElement(eSide)->getNrOfBasisFunctions();
	}

	//Create datastructures
	LinearAlgebra::Matrix stabilityParametersAverage(instance_.numOfVariables_,instance_.DIM_);
	LinearAlgebra::NumericalVector stabilityParametersInternal(instance_.numOfVariables_);
	LinearAlgebra::NumericalVector stabilityParametersExternal(instance_.numOfVariables_);
	LinearAlgebra::NumericalVector rhsInternal(numOfTestBasisFunctionsInternal);
	LinearAlgebra::NumericalVector rhsExternal(numOfTestBasisFunctionsExternal);
	LinearAlgebra::NumericalVector stabilityParameterCoefficientsInternal;
	LinearAlgebra::NumericalVector stabilityParameterCoefficientsExternal;



	//Compute the flux functions as function of iE and iD
	computeStabilityFluxFunction(ATensorInternal, ATensorExternal, stateInternal, stateExternal, normalInternal);

	computeStabilityFluxFunction(ATensorInternal, ATensorExternal, stateInternal, stateExternal,  normalInternal);

	for (std::size_t iD = 0; iD < instance_.DIM_; iD++)
	{
			//Integrate the correct fluxfunction
			rhsInternal = computeRhs(ptrFace, iSide, stabilityFluxFunctionInternal_, iD);
			rhsExternal = computeRhs(ptrFace, eSide, stabilityFluxFunctionExternal_, iD);

			//Solve the stabilityParameter coefficients
			stabilityMassMatrix_.solve(rhsInternal);
			stabilityMassMatrix_.solve(rhsExternal);

			//Reconstruct the stabilityparameter coefficients for a given iD
			stabilityParametersInternal = instance_.computeSolutionOnFace(ptrFace, iSide, rhsInternal, pRef);
			stabilityParametersExternal = instance_.computeSolutionOnFace(ptrFace, eSide, rhsExternal, pRef);

			//Put result into matrix
			for (std::size_t iE = 0; iE < instance_.numOfVariables_; iE++)
			{
				stabilityParametersAverage(iE,iD) = 0.5*(stabilityParametersInternal(iE) + stabilityParametersExternal(iE));
			}
	}

	return stabilityParametersAverage;
}

std::vector<LinearAlgebra::Matrix> Viscous::computeATensor( const LinearAlgebra::NumericalVector partialState, const double viscosity)
{
	//todo: note that the kinetic velocity must be computed for a wide range of problems. Fix this.
	//todo: remove the kinetic velocity in this computation.
	//todo: Some terms show up several times, i.e. 2/3mu and 4/3. make this faster
	//todo: division by rho has to be computed an aweful lot of times
	//todo: Check if this also works correctly in 3D

	std::vector<LinearAlgebra::Matrix> ATensor(instance_.DIM_*instance_.DIM_);
	double velocityNormSquared;
	double thermalFactor = instance_.gamma_*viscosity/PrInv_;

	double pos1;
	double pos2;

	for (std::size_t iD = 0; iD < instance_.DIM_; iD++)
	{
		velocityNormSquared += partialState(iD+1)*partialState(iD+1);
	}

	LinearAlgebra::Matrix APartial1(instance_.DIM_+2,instance_.DIM_+2);
	LinearAlgebra::Matrix APartial2(instance_.DIM_+2,instance_.DIM_+2);

	//A11 A22 en A33: For documentation see the full matrix in Klaij et al. 2006
	for (std::size_t iD = 0; iD < instance_.DIM_; iD++)
	{
		//Reset matrix
		APartial1 *= 0.0;

		//Tensor index
		pos1 = (instance_.DIM_)*iD + iD;

		// (1) viscosity contributions
		for (std::size_t iD2 = 0; iD2 < instance_.DIM_; iD2++)
		{
			APartial1(iD2+1,iD2+1) = viscosity;
			APartial1(iD2+1,0) = -viscosity*partialState(iD2+1);
			APartial1(instance_.DIM_+1,iD2+1) = viscosity;
		}

		// Multiply by correct value of the dominant component
		APartial1(iD+1,0) *= 4.0/3.0;
		APartial1(iD+1,iD+1) *= 4.0/3.0;
		APartial1(instance_.DIM_+1,iD+1) *= 4.0/3.0;

		// (2) temperature contribution
		for (std::size_t iD2 = 0; iD2 < instance_.DIM_; iD2++)
		{
			APartial1(instance_.DIM_+1,iD2+1) += -thermalFactor;
		}
		APartial1(instance_.DIM_+1,instance_.DIM_+1) = thermalFactor;

		// Complete energy part by multiplying by velocity.
		for (std::size_t iD2 = 0; iD2 < instance_.DIM_; iD2++)
		{
			APartial1(instance_.DIM_+1,iD2+1) *= partialState(iD2+1);
		}

		APartial1(instance_.DIM_+1,0) = -(1.0/3.0)*partialState(iD+1)*partialState(iD+1) - viscosity*velocityNormSquared - thermalFactor*(partialState(instance_.DIM_+1) - velocityNormSquared);

		//Divide by rho
		APartial1 *= 1.0/partialState(0);
		ATensor[pos1] = APartial1;
	}

	//A12 A13 A23 ? Note: A12 --> A(iD1)(iD2)
	for (std::size_t iD1 = 0; iD1 < instance_.DIM_ - 1; iD1++)
	{
		for (std::size_t iD2 = iD1 + 1; iD2 < instance_.DIM_; iD2++)
		{
			//Reset matrix
			APartial1 *= 0.0;
			APartial2 *= 0.0;

			//Tensor index
			pos1 = (instance_.DIM_)*iD1 + iD2;
			pos2 = (instance_.DIM_)*iD2 + iD1;

			// viscosity contributions for A(iD1)(iD2)
			APartial1(iD1+1,0) = 2.0/3.0*viscosity*partialState(iD2+1);
			APartial1(iD2+1,0) = -viscosity*partialState(iD1+1);

			APartial1(iD1+1,iD2+1) = -2.0/3.0*viscosity;
			APartial1(iD2+1,iD1+1) =  viscosity;

			APartial1(instance_.DIM_ + 1, 0) = -1.0/3.0*viscosity*partialState(iD1+1)*partialState(iD2+1);
			APartial1(instance_.DIM_ + 1, iD2+1) = -2.0/3.0*viscosity*partialState(iD1+1);
			APartial1(instance_.DIM_ + 1, iD1+1) = viscosity*partialState(iD2+1);

			// viscosity contributions for A(iD2)(iD1)
			APartial2(iD1+1,0) = -viscosity*partialState(iD2+1);
			APartial2(iD2+1,0) = 2.0/3.0*viscosity*partialState(iD1+1);

			APartial2(iD1+1,iD2+1) = viscosity;
			APartial2(iD2+1,iD1+1) =  -2.0/3.0*viscosity;

			APartial2(instance_.DIM_ + 1, 0) = -1.0/3.0*viscosity*partialState(iD1+1)*partialState(iD2+1);
			APartial2(instance_.DIM_ + 1, iD2+1) = viscosity*partialState(iD1+1);
			APartial2(instance_.DIM_ + 1, iD1+1) = -2.0/3.0*viscosity*partialState(iD2+1);

			//Divide by rho
			APartial1 *= 1/partialState(0);
			APartial2 *= 1/partialState(0);

			//Assign matrices to the tensor vector
			ATensor[pos1] = APartial1;
			ATensor[pos2] = APartial2;

		}
	}
	return ATensor;
}

LinearAlgebra::Matrix Viscous::computeATensorMatrixContraction(const std::vector<LinearAlgebra::Matrix> ATensor, const LinearAlgebra::Matrix matrix)
{
	//todo: verify this function
	//todo: this is the naief implementation: write a quicker one. There are many zero multiplications in this function. That is a waste of computational effort
	//todo: this is probably quicker in fortran
	//A_ikrs = A_(iV)(iD)(iVm)(iDm)
	LinearAlgebra::Matrix result(instance_.numOfVariables_,instance_.DIM_);
	double pos;

	for (std::size_t iD = 0; iD < instance_.DIM_; iD++)
	{
		for (std::size_t iV = 0; iV < instance_.numOfVariables_; iV++)
		{
			for (std::size_t iDm = 0; iDm < instance_.DIM_; iDm++)
			{
				for (std::size_t iVm = 0; iVm < instance_.numOfVariables_; iVm++)
				{
					pos = (instance_.DIM_)*iD + iDm;
					result(iV,iD) += ATensor[pos](iV,iVm)*matrix(iVm,iDm);
				}
			}
		}
	}

	return result;
}

LinearAlgebra::NumericalVector Viscous::integrandAtElement(const Base::Element *ptrElement, const LinearAlgebra::NumericalVector qSolution, const LinearAlgebra::Matrix qSolutionJacobian, const double pressure, const LinearAlgebra::NumericalVector partialState, const Geometry::PointReference &pRef)
{
	// Get the number of basis functions in an element.
	std::size_t numOfBasisFunctions = ptrElement->getNrOfBasisFunctions();

	// Create data structures for calculating the integrand
	LinearAlgebra::NumericalVector integrand(instance_.numOfVariables_ * numOfBasisFunctions);
	LinearAlgebra::NumericalVector gradientBasisFunction;
	std::size_t iVB;


	// Compute A tensor
	double temperature = computeTemperature(qSolution, pressure);
	double viscosity = computeViscosity(temperature);
	ATensorInternal_ = computeATensor(partialState, viscosity);

	// Compute flux (A_ikrs*Ur,s)
	LinearAlgebra::Matrix fluxFunction = computeATensorMatrixContraction(ATensorInternal_, qSolutionJacobian);

	for (std::size_t iB = 0; iB < numOfBasisFunctions; iB++) // for all basis functions
	{
		gradientBasisFunction = ptrElement->basisFunctionDeriv(iB,pRef);
		for (std::size_t iD = 0; iD < instance_.DIM_; iD++) // for all dimensions
		{
			for (std::size_t iE = 0; iE < instance_.DIM_ + 2; iE++) // for all equations
			{
				iVB = ptrElement->convertToSingleIndex(iB,iE);
				integrand(iVB) += fluxFunction(iE,iD)*gradientBasisFunction(iD);
			}
		}
	}

	return integrand;
}

LinearAlgebra::NumericalVector Viscous::integrandViscousAtFace(const Base::Face *ptrFace, const Base::Side &iSide, LinearAlgebra::NumericalVector qSolutionInternal, LinearAlgebra::NumericalVector qSolutionExternal, double pressure, LinearAlgebra::NumericalVector partialState, const LinearAlgebra::NumericalVector normal, const Geometry::PointReference &pRef)
{
	//Compute velocity normal matrix
	LinearAlgebra::Matrix velocityNormal(instance_.DIM_+2,instance_.DIM_);
	LinearAlgebra::NumericalVector qSolutionDifference;
	qSolutionDifference = qSolutionInternal - qSolutionExternal;
	for (std::size_t iV = 0; iV < instance_.numOfVariables_; iV++)
	{
		for (std::size_t iD = 0; iD < instance_.DIM_; iD++)
		{
			velocityNormal(iV,iD) = 0.5*qSolutionDifference(iV)*normal(iD);
		}
	}

	//Compute A and A contraction with velocitynormal matrix
	double temperature = computeTemperature(qSolutionInternal, pressure);
	double viscosity = computeViscosity(temperature);

	ATensorInternal_ = computeATensor(partialState, viscosity);
	LinearAlgebra::Matrix fluxFunction = computeATensorMatrixContraction(ATensorInternal_, velocityNormal);

	//Compute integrand
	std::size_t numOfTestBasisFunctions = ptrFace->getPtrElement(iSide)->getNrOfBasisFunctions();
	LinearAlgebra::NumericalVector integrand(instance_.numOfVariables_*numOfTestBasisFunctions);
	LinearAlgebra::NumericalVector gradientBasisFunction;
	std::size_t iEB; // Index for both variable and basis function.

	for (std::size_t iB = 0; iB < numOfTestBasisFunctions; iB++) // for all basis functions
	{
		gradientBasisFunction = ptrFace->basisFunctionDeriv(iSide,iB,pRef);
		for (std::size_t iD = 0; iD < instance_.DIM_; iD++) // for all dimensions
		{
			for (std::size_t iE = 0; iE < instance_.DIM_ + 2; iE++) // for all equations
			{
				iEB = ptrFace->getPtrElement(iSide)->convertToSingleIndex(iB,iE);
				integrand(iEB) += fluxFunction(iE,iD)*gradientBasisFunction(iD);
			}
		}
	}
	return -integrand;
}

LinearAlgebra::Matrix Viscous::computeAuxilliaryFlux(
		const Base::Face *ptrFace,
		Base::Side iSide,
		const LinearAlgebra::NumericalVector stateInternal,
		const LinearAlgebra::NumericalVector stateExternal,
		const double pressureInternal,
		const double pressureExternal,
		const LinearAlgebra::NumericalVector partialStateInternal,
		const LinearAlgebra::NumericalVector partialStateExternal,
		const LinearAlgebra::Matrix stateJacobianInternal,
		const LinearAlgebra::Matrix stateJacobianExternal,
		const LinearAlgebra::NumericalVector normalInternal,
		const Geometry::PointReference &pRef)
{
	double eta = 3.0; //stability value hard coded
	LinearAlgebra::Matrix fluxInternal;
	LinearAlgebra::Matrix fluxExternal;
	LinearAlgebra::Matrix stabilityFlux;
	LinearAlgebra::Matrix AuxilliaryFlux;

	//Compute A and A contraction with velocitynormal matrix
	double temperatureInternal = computeTemperature(stateInternal, pressureInternal);
	double viscosityInternal = computeViscosity(temperatureInternal);


	double temperatureExternal = computeTemperature(stateExternal, pressureExternal);
	double viscosityExternal = computeViscosity(temperatureExternal);

	ATensorInternal_ = computeATensor(partialStateInternal, viscosityInternal);
	ATensorExternal_ = computeATensor(partialStateExternal, viscosityExternal);

	fluxInternal  = computeATensorMatrixContraction(ATensorInternal_, stateJacobianInternal);
	fluxExternal = computeATensorMatrixContraction(ATensorExternal_, stateJacobianInternal);
	stabilityFlux = computeStabilityParameters(ptrFace, iSide, ATensorInternal_, ATensorExternal_, stateInternal, stateExternal, normalInternal, pRef);

	AuxilliaryFlux = 0.5*(fluxInternal + fluxExternal) - eta*stabilityFlux;

	return AuxilliaryFlux;
}

LinearAlgebra::NumericalVector Viscous::integrandAuxilliaryAtFace(const Base::Face *ptrFace, const Base::Side &iSide, LinearAlgebra::NumericalVector stateInternal, const LinearAlgebra::NumericalVector stateExternal, const double pressureInternal, const double pressureExternal, const LinearAlgebra::NumericalVector partialStateInternal, const LinearAlgebra::NumericalVector partialStateExternal, const LinearAlgebra::NumericalVector normalInternal, const LinearAlgebra::Matrix stateJacobianInternal, const LinearAlgebra::Matrix stateJacobianExternal, const Geometry::PointReference &pRef)
{
	LinearAlgebra::Matrix fluxFunction = computeAuxilliaryFlux(ptrFace, iSide, stateInternal, stateExternal, pressureInternal, pressureExternal, partialStateInternal, partialStateExternal, stateJacobianInternal, stateJacobianExternal, normalInternal, pRef);

	//Compute integrand
	std::size_t numOfTestBasisFunctions = ptrFace->getPtrElement(iSide)->getNrOfBasisFunctions();
	LinearAlgebra::NumericalVector integrand(instance_.numOfVariables_*numOfTestBasisFunctions);
	LinearAlgebra::NumericalVector partialIntegrand(instance_.numOfVariables_);
	std::size_t iEB;

	for (std::size_t iV = 0; iV < instance_.numOfVariables_; iV++)
	{
		for (std::size_t iD = 0; iD < instance_.DIM_; iD++)
		{
			partialIntegrand(iV) += fluxFunction(iV,iD)*normalInternal(iD);
		}
	}

	for (std::size_t iB = 0; iB < numOfTestBasisFunctions; iB++)
	{
		for (std::size_t iE = 0; iE < instance_.DIM_; iE++)
		{
			iEB = ptrFace->getPtrElement(iSide)->convertToSingleIndex(iB,iE);
			integrand(iEB) += ptrFace->basisFunction(iSide, iB, pRef)*partialIntegrand(iE);
		}
	}

	return -integrand;
}




























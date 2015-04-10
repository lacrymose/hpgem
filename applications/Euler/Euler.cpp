/*
 * Euler.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: marnix
 */

#include "Euler.h"
#include <cmath>

Euler::Euler
(
 const std::size_t dimension,
 const std::size_t numOfVariables,
 const std::size_t polynomialOrder,
 const Base::ButcherTableau * const ptrButcherTableau
) :
HpgemAPISimplified(dimension, numOfVariables, polynomialOrder, ptrButcherTableau),
DIM_(dimension),
numOfVariables_(numOfVariables)
{
}


Base::RectangularMeshDescriptor Euler::createMeshDescription(const std::size_t numOfElementPerDirection)
{
    // Create the domain. In this case the domain is the square [0,1]^DIM and periodic.
    Base::RectangularMeshDescriptor description(DIM_);
    for (std::size_t i = 0; i < DIM_; ++i)
    {
        description.bottomLeft_[i] = 0;
        description.topRight_[i] = 1;
        description.numElementsInDIM_[i] = numOfElementPerDirection;
        description.boundaryConditions_[i] = Base::BoundaryType::PERIODIC;
    }

    return description;
}

/// \brief Compute the initial solution at a given point in space and time.
LinearAlgebra::NumericalVector Euler::getInitialSolution(const PointPhysicalT &pPhys, const double &startTime, const std::size_t orderTimeDerivative)
{
	LinearAlgebra::NumericalVector initialSolution(numOfVariables_);

	//density and energy density:
	initialSolution(0) = 2.0 + 0.1*std::sin(M_PI*pPhys[0])*std::sin(M_PI*pPhys[1]);
	initialSolution(1) = 0.0;
	initialSolution(2) = 0.0;
	initialSolution(DIM_+1) = 20.0;

    return initialSolution;
}

/// *****************************************
/// ***   Element integration functions   ***
/// *****************************************

LinearAlgebra::NumericalVector Euler::integrandRightHandSideOnRefElement(const Base::Element *ptrElement, const double &time, const Geometry::PointReference &pRef, const LinearAlgebra::NumericalVector &solutionCoefficients)
{
//Select the correct integrand with respect to the dimension
	switch (DIM_)
	{
	case 1:
		logger(ERROR, "ERROR: 1D not implemented yet.");
		return integrandRightHandSideOnRefElement2D(ptrElement, time, pRef, solutionCoefficients);
	case 2:
		return integrandRightHandSideOnRefElement2D(ptrElement, time, pRef, solutionCoefficients);
	case 3:
		logger(ERROR, "ERROR: 3D not implemented yet.");
		return integrandRightHandSideOnRefElement2D(ptrElement, time, pRef, solutionCoefficients);
	default:
		logger(ERROR, "ERROR: dimension of the problem is not correctly chosen. Dimension is reset to 2D.");
		return integrandRightHandSideOnRefElement2D(ptrElement, time, pRef, solutionCoefficients);
	}
}

LinearAlgebra::NumericalVector Euler::integrandRightHandSideOnRefElement1D(const Base::Element *ptrElement, const double &time, const Geometry::PointReference &pRef, const LinearAlgebra::NumericalVector &solutionCoefficients)
{
	// Get the number of basis functions in an element.
	std::size_t numOfBasisFunctions =  ptrElement->getNrOfBasisFunctions();

	//Create data structures for calculating the integrand
	LinearAlgebra::NumericalVector integrand(numOfVariables_ * numOfBasisFunctions); //The final integrand value will be stored in this vector

	//todo: Create this function
    return integrand;

	//Questions: is integrand defined as a zero function on initialisation?

}

LinearAlgebra::NumericalVector Euler::integrandRightHandSideOnRefElement2D(const Base::Element *ptrElement, const double &time, const Geometry::PointReference &pRef, const LinearAlgebra::NumericalVector &solutionCoefficients)
{
	//This function should work for all dimensions.
	// Get the number of basis functions in an element.
	std::size_t numOfBasisFunctions =  ptrElement->getNrOfBasisFunctions();

	//Create data structures for calculating the integrand
	LinearAlgebra::NumericalVector integrand(numOfVariables_ * numOfBasisFunctions); //The final integrand value will be stored in this vector
	LinearAlgebra::NumericalVector qReconstructed(numOfVariables_); //The reconstructed solution values are stored here for every variable (u = sum_i u_i*phi_i)
	LinearAlgebra::NumericalVector gradientBasisFunction(DIM_); //Gradient function based on the number of dimensions
	double pressureTerm;
	double enthalpyTerm;

	//Create temporary result values
	double reconstruction; // Used to iteratively construct the Solution reconstruction
	double integrandTerm;

	//Create iteration values
	std::size_t iVB; // Index in solutioncoefficients for variable i and basisfunction j

	//Compute the reconstruction of the solution
	// 0 --> q1
	// 1 --> q2
	// 2 --> q3
	// 3 --> q4
	for(std::size_t iV = 0; iV < numOfVariables_; iV++)
	{
		reconstruction = 0;
		for (std::size_t iB = 0; iB < numOfBasisFunctions; iB++)
		{
			iVB = ptrElement->convertToSingleIndex(iB,iV);
			reconstruction += solutionCoefficients(iVB)*ptrElement->basisFunction(iB, pRef);
		}

		qReconstructed(iV) = reconstruction; // q are the reconstructed variables.
	}

	//Compute pressure term
	double q1Inverse = 1.0/qReconstructed(0);
	// ******** IMPORTANT ************* pressure is only defined for 2d here.
	pressureTerm = (gamma_ -1)*(qReconstructed(3) - 0.5*q1Inverse*(qReconstructed(1)*qReconstructed(1) + qReconstructed(2)*qReconstructed(2))); // (gamma-1)*rho*(e- (u^2 + v^2)/2)

	if(pressureTerm < 0.0)
	{
		std::cout << "Element number: " << ptrElement->getID() << std::endl;
		std::cout << "Time: " << time << std::endl;
		std::cout << "density: " << qReconstructed(0) << std::endl;
		std::cout << "momentum x-direction: " << qReconstructed(1) << std::endl;
		std::cout << "momentum y-direction: " << qReconstructed(2) << std::endl;
		std::cout << "density times energy: " << qReconstructed(3) << std::endl;
		std::cout << "Pressure: " << pressureTerm << std::endl;
		logger(ERROR,"ERROR: Negative pressure");
	}

	if (qReconstructed(0) < 0.0)
	{
		std::cout << "Element number: " << ptrElement->getID() << std::endl;
		std::cout << "Time: " << time << std::endl;
		std::cout << "density: " << qReconstructed(0) << std::endl;
		std::cout << "momentum x-direction: " << qReconstructed(1) << std::endl;
		std::cout << "momentum y-direction: " << qReconstructed(2) << std::endl;
		std::cout << "density times energy: " << qReconstructed(3) << std::endl;
		logger(ERROR,"ERROR: Negative density");
	}

	// Compute the integrand for all equations
	//************ THIS FUNCTION IS ONLY DEFINED FOR 2D *****************
	for (std::size_t iB = 0; iB < numOfBasisFunctions; iB++) // For every basis function
	{
		gradientBasisFunction = ptrElement->basisFunctionDeriv(iB, pRef); // Compute the gradient of the ith test function

		for (std::size_t iD = 0; iD < DIM_; iD++) // For every dimension
		{

			//Calculate convection terms

			integrandTerm = qReconstructed(iD+1)*gradientBasisFunction(iD)*q1Inverse; // (rho*u)*d(phi_iB)/dx or (rho*v)*d(phi_iB)/dy

			//q1 integrand
			iVB = ptrElement->convertToSingleIndex(iB,0);
			integrand(iVB) += qReconstructed(iD+1)*gradientBasisFunction(iD); // rho*u*d(phi)/dx + rho*v*d(phi)/dy

			//q2 integrand
			iVB = ptrElement->convertToSingleIndex(iB,1);
			integrand(iVB) += qReconstructed(1)*integrandTerm; //rho*u*u*d(phi)/dx + rho*u*v*d(phi)/dy

			//Momentum y-direction integrand
			iVB = ptrElement->convertToSingleIndex(iB,2);
			integrand(iVB) += qReconstructed(2)*integrandTerm; //rho*v*u*d(phi)/dx + rho*v*v*d(phi)/dy

			//Energy integrand
			iVB = ptrElement->convertToSingleIndex(iB,3);
			integrand(iVB) += (qReconstructed(3) + pressureTerm)*integrandTerm;

			//Calculate pressure terms in momentum equations
			iVB = ptrElement->convertToSingleIndex(iB,iD+1);
			integrand(iVB) += pressureTerm*gradientBasisFunction(iD);

		}
	}



    // Scale with the reference-to-physical element ratio.
    Geometry::Jacobian jac = ptrElement->calcJacobian(pRef);
    integrand *= jac.determinant();

    ///\todo it is better to use assert for debug checks like this
   	if (integrand(0) != integrand(0))
   	{
   		logger(ERROR,"ERROR: NaN found in element integrand.");
   	}
    return integrand;

}

LinearAlgebra::NumericalVector Euler::integrandRightHandSideOnRefElement3D(const Base::Element *ptrElement, const double &time, const Geometry::PointReference &pRef, const LinearAlgebra::NumericalVector &solutionCoefficients)
{
	// Get the number of basis functions in an element.
	std::size_t numOfBasisFunctions =  ptrElement->getNrOfBasisFunctions();

	//Create data structures for calculating the integrand
	LinearAlgebra::NumericalVector integrand(numOfVariables_ * numOfBasisFunctions); //The final integrand value will be stored in this vector

	//todo: Create this function
    return integrand;

	//Questions: is integrand defined as a zero function on initialisation?

}

LinearAlgebra::NumericalVector Euler::computeRightHandSideAtElement(Base::Element *ptrElement, LinearAlgebra::NumericalVector &solutionCoefficients, const double time)
{
    //Define the integrand function for the right hand side for the reference element.
    std::function<LinearAlgebra::NumericalVector(const Geometry::PointReference &)> integrandFunction = [=](const Geometry::PointReference & pRef) -> LinearAlgebra::NumericalVector
    {   return this->integrandRightHandSideOnRefElement(ptrElement, time, pRef, solutionCoefficients);};

    return elementIntegrator_.referenceElementIntegral(ptrElement->getGaussQuadratureRule(), integrandFunction);

}

/// *****************************************
/// ***    face integration functions     ***
/// *****************************************

   //Compute the Roe Riemann Flux function
   LinearAlgebra::NumericalVector Euler::RoeRiemannFluxFunction(const LinearAlgebra::NumericalVector &qReconstructionLeft, const LinearAlgebra::NumericalVector &qReconstructionRight, LinearAlgebra::NumericalVector &normal, const Base::Side &iSide)
   {

	   //Compute correct normal direction and difference vector
	   LinearAlgebra::NumericalVector qDifference = qReconstructionLeft - qReconstructionRight;

	   //Compute normal unit vector
	   LinearAlgebra::NumericalVector normalUnit;
	   normalUnit = normal;//*areaInverse;

	   //Compute the Roe average state
	   LinearAlgebra::NumericalVector qAverage(DIM_+1);
	   double zL = std::sqrt(qReconstructionLeft(0));
	   double zR = std::sqrt(qReconstructionRight(0));
	   double tmp1 = 1.0/(qReconstructionLeft(0) + zL*zR);
	   double tmp2 = 1.0/(qReconstructionRight(0) + zL*zR);
	   double ruSquaredLeft = 0.0;
	   double ruSquaredRight = 0.0;
	   double rhoInverseLeft = 1.0/qReconstructionLeft(0);
	   double rhoInverseRight = 1.0/qReconstructionRight(0);
	   double pressureLeft;
	   double pressureRight;

	   for (std::size_t iD = 0; iD < DIM_; iD++)
	   {
		   qAverage(iD) = qReconstructionLeft(iD+1)*tmp1 + qReconstructionRight(iD+1)*tmp2; // u_average
		   ruSquaredLeft += qReconstructionLeft(iD+1)*qReconstructionLeft(iD+1); 	// Kinetic part of pressure term
		   ruSquaredRight += qReconstructionRight(iD+1)*qReconstructionRight(iD+1);
	   }

	   pressureLeft = (gamma_ - 1)*(qReconstructionLeft(DIM_ + 1) - 0.5*ruSquaredLeft*rhoInverseLeft);
	   pressureRight = (gamma_ - 1)*(qReconstructionRight(DIM_ + 1) - 0.5*ruSquaredRight*rhoInverseRight);

	    if (pressureLeft < 0.0)
	    {
	    	logger(ERROR,"Error: Pressure left is zero");
	    }
	    if (pressureRight < 0.0)
	    {
	    	std::cout << "Density energy: " << qReconstructionRight(DIM_ + 1) << std::endl;
	    	std::cout << "kinetic energy: " << ruSquaredRight << std::endl;
	    	logger(ERROR,"Error: Pressure right is zero");
	    }

	   qAverage(DIM_) = (qReconstructionLeft(DIM_+1) + pressureLeft)*tmp1 + (qReconstructionRight(DIM_+1) + pressureRight)*tmp2; //H_average

	   //Compute useful variables for constructing the flux function
	   double alphaAvg = 0.0;
	   double unAvg = 0.0;
	   for (std::size_t iD = 0; iD < DIM_; iD++)
	   {
		   alphaAvg += (qAverage(iD)*qAverage(iD));
		   unAvg += qAverage(iD)*normalUnit(iD);
	   }
	   alphaAvg *= 0.5;

	   if (unAvg != unAvg)
	   {
		   std::cout << "unAvg: " << unAvg << std::endl;
		   std::cout << "qAverage: " << qAverage << std::endl;
		   std::cout << "Pressure left: " << pressureLeft << std::endl;
		   std::cout << "Pressure right: " << pressureRight << std::endl;
		   std::cout << "qReconstructionLeft: " << qReconstructionLeft << std::endl;
		   std::cout << "qReconstructionRight: " << qReconstructionRight << std::endl;
		   logger(ERROR,"Error: NaN speed.");
	   }


	   const double a2Avg = std::abs((gamma_ -1)*(qAverage(DIM_) - alphaAvg));
	   const double aAvg = std::sqrt(a2Avg);
	   const double ovaAvg = 1.0/aAvg;
	   const double ova2Avg = 1.0/a2Avg;

	   //Compute eigenvalues
	   double lam1 = std::abs(unAvg + aAvg);
	   double lam2 = std::abs(unAvg - aAvg);
	   double lam3 = std::abs(unAvg);

	   //std::cout << "unAvg: " << unAvg << std::endl;
	   //std::cout << "aAvg: " << aAvg << std::endl;

	   //Add entropy correction
	   double epsilon = 0.01;
	   if (lam1 < epsilon)
	   {
		   lam1 = (lam1*lam1 + epsilon*epsilon)/(2.0*epsilon);
	   }

	   if (lam2 < epsilon)
	   {
		   lam2 = (lam2*lam2 + epsilon*epsilon)/(2.0*epsilon);
	   }

	   if (lam3 < epsilon)
	   {
		   lam3 = (lam3*lam3 + epsilon*epsilon)/(2.0*epsilon);
	   }

	   //Compute useful abbreviations for constructing the flux function
	   const double abv1 = 0.5*(lam1 + lam2);
	   const double abv2 = 0.5*(lam1 - lam2);
	   const double abv3 = abv1 - lam3;

	   double abv4 = alphaAvg*qDifference(0);
	   double abv5 = -unAvg*qDifference(0);
	   for (std::size_t iD = 0; iD < DIM_; iD++)
	   {
		   abv4 += -qAverage(iD)*qDifference(iD+1);
		   abv5 += normalUnit(iD)*qDifference(iD+1);
	   }
	   abv4 += qDifference(DIM_+1);
	   abv4 *= (gamma_ -1);

	   const double abv6 = abv3*abv4*ova2Avg + abv2*abv5*ovaAvg;
	   const double abv7 = abv2*abv4*ovaAvg + abv3*abv5;

	   //Compute the Roe Riemann Flux function
	   LinearAlgebra::NumericalVector flux(DIM_ + 2);


	    double pLR = pressureLeft + pressureRight;

	    double runR = 0.0;
	    double runL = 0.0;
	    for (std::size_t iD = 0; iD < DIM_; iD++)
	    {
	    	runL += qReconstructionLeft(iD+1)*normal(iD);
	    	runR += qReconstructionRight(iD+1)*normal(iD);
	    }

	    double unL = runL*rhoInverseLeft;
	    double unR = runR*rhoInverseRight;

	    //continuity equation
	    flux(0) = (runL + runR - (lam3*qDifference(0) + abv6));

	    //momentum equations
	    for (std::size_t iD = 0; iD < DIM_; iD++)
	    {
	    	flux(iD+1) = runL*qReconstructionLeft(iD+1)*rhoInverseLeft + runR*qReconstructionRight(iD+1)*rhoInverseRight + pLR*normal(iD) - (lam3*qDifference(iD+1) + qAverage(iD)*abv6 + normalUnit(iD)*abv7);
	    }
	    //energy equations
	    flux(DIM_+1) = (unL*(qReconstructionLeft(DIM_+1)+pressureLeft) + unR*(qReconstructionRight(DIM_+1)+pressureRight) - (lam3*qDifference(DIM_+1) + qAverage(DIM_)*abv6 + unAvg*abv7));

	    //Note: Twice the flux is computed above, hence the factor 0.5 in front of the equation
	   return 0.5*flux;
   }

   /// \brief Compute the integrand for the right hand side for the reference face corresponding to an external face.
   LinearAlgebra::NumericalVector Euler::integrandRightHandSideOnRefFace(const Base::Face *ptrFace, const double &time, const Geometry::PointReference &pRef, const LinearAlgebra::NumericalVector &solutionCoefficients)
   {
	   //Get the number of basis functions
	   std::size_t numOfBasisFunctionsLeft= ptrFace->getPtrElementLeft()->getNrOfBasisFunctions(); //Get the number of basis functions on the left

	   LinearAlgebra::NumericalVector integrand(numOfVariables_*numOfBasisFunctionsLeft);
	   LinearAlgebra::NumericalVector qReconstructionLeft(numOfVariables_);
	   LinearAlgebra::NumericalVector qReconstructionRight(numOfVariables_);

	   //Compute left and right states
	    std::size_t jVB; // Index for both variable and basis function.
	    for (std::size_t jV = 0; jV < numOfVariables_; jV++)
	    {
	        qReconstructionLeft(jV) = 0;
	        for (std::size_t jB = 0; jB < numOfBasisFunctionsLeft; jB++)
	        {
	            jVB = ptrFace->getPtrElementLeft()->convertToSingleIndex(jB, jV);
	            qReconstructionLeft(jV) += ptrFace->basisFunction(Base::Side::LEFT, jB, pRef) * solutionCoefficients(jVB);
	        }
	    }

		if (qReconstructionLeft(0) < 0.0)
		{
			std::cout << "Element number: " << ptrFace->getID() << std::endl;
			std::cout << "Time: " << time << std::endl;
			std::cout << "density: " << qReconstructionLeft(0) << std::endl;
			std::cout << "momentum x-direction: " << qReconstructionLeft(1) << std::endl;
			std::cout << "momentum y-direction: " << qReconstructionLeft(2) << std::endl;
			std::cout << "density times energy: " << qReconstructionLeft(3) << std::endl;
			logger(ERROR,"ERROR: Negative energy left of the face");
		}

		if (qReconstructionRight(0) < 0.0)
		{
			std::cout << "Element number: " << ptrFace->getID() << std::endl;
			std::cout << "Time: " << time << std::endl;
			std::cout << "density: " << qReconstructionRight(0) << std::endl;
			std::cout << "momentum x-direction: " << qReconstructionRight(1) << std::endl;
			std::cout << "momentum y-direction: " << qReconstructionRight(2) << std::endl;
			std::cout << "density times energy: " << qReconstructionRight(3) << std::endl;
			logger(ERROR,"ERROR: Negative energy right of the face");
		}

	   // Boundary face is assumed to be a solid wall: set reflective solution on the other side
	   qReconstructionRight(0) = qReconstructionLeft(0);
	   for (std::size_t iD = 0; iD < DIM_; iD++)
	   {
		   qReconstructionRight(iD+1) = -qReconstructionLeft(iD+1);
	   }
	   qReconstructionRight(DIM_+1) = qReconstructionLeft(DIM_+1);

	   // Compute normal vector, with size of the ref-to-phys face scale, pointing outward of the left element.
	   LinearAlgebra::NumericalVector normal = ptrFace->getNormalVector(pRef);


	   //Compute flux
	   LinearAlgebra::NumericalVector flux = RoeRiemannFluxFunction(qReconstructionLeft, qReconstructionRight, normal, Base::Side::LEFT);

	   // Compute integrand on the reference element.
	   std::size_t iVB; // Index for both variable and basis function.
	   for (std::size_t iB = 0; iB < numOfBasisFunctionsLeft; iB++)
	   {
	       for (std::size_t iV = 0; iV < numOfVariables_; iV++) // Index for direction
	       {
	           iVB = ptrFace->getPtrElementLeft()->convertToSingleIndex(iB, iV);
	           integrand(iVB) = -flux(iV)*ptrFace->basisFunction(Base::Side::LEFT, iB, pRef);
	       }
	   }

	   	if (integrand(0) != integrand(0))
	   	{
	   		std::cout << "The flux: " << -flux << std::endl;
	   		std::cout << "qReconstructionRight: " << qReconstructionRight << std::endl;
	   		std::cout << "qReconstructionLeft: " << qReconstructionLeft << std::endl;
	   		logger(ERROR,"ERROR: NaN found in face integrand.");
	   	}
	   return  integrand;
   }

   /// \brief Compute the integrand for the right hand side for the reference face corresponding to an internal face.
   LinearAlgebra::NumericalVector Euler::integrandRightHandSideOnRefFace(const Base::Face *ptrFace, const double &time, const Geometry::PointReference &pRef, const Base::Side &iSide, const LinearAlgebra::NumericalVector &solutionCoefficientsLeft, const LinearAlgebra::NumericalVector &solutionCoefficientsRight)
   {
	   //Get the number of basis functions
	   std::size_t numOfTestBasisFunctions = ptrFace->getPtrElement(iSide)->getNrOfBasisFunctions(); // Get the number of test basis functions on a given side, iSide
	   std::size_t numOfSolutionBasisFunctionsLeft = ptrFace->getPtrElementLeft()->getNrOfBasisFunctions(); //Get the number of basis functions on the left
	   std::size_t numOfSolutionBasisFunctionsRight = ptrFace->getPtrElementRight()->getNrOfBasisFunctions(); //Get the number of basis functions on the right side


	   LinearAlgebra::NumericalVector integrand(numOfVariables_*numOfTestBasisFunctions);
	   LinearAlgebra::NumericalVector qReconstructionLeft(numOfVariables_);
	   LinearAlgebra::NumericalVector qReconstructionRight(numOfVariables_);

	   //Compute left and right states
	    std::size_t jVB; // Index for both variable and basis function.
	    for (std::size_t jV = 0; jV < numOfVariables_; jV++)
	    {
	        qReconstructionLeft(jV) = 0;
	        qReconstructionRight(jV) = 0;
	        for (std::size_t jB = 0; jB < numOfSolutionBasisFunctionsLeft; jB++)
	        {
	            jVB = ptrFace->getPtrElementLeft()->convertToSingleIndex(jB, jV);
	            qReconstructionLeft(jV) += ptrFace->basisFunction(Base::Side::LEFT, jB, pRef) * solutionCoefficientsLeft(jVB);
	        }
	        for (std::size_t jB = 0; jB < numOfSolutionBasisFunctionsRight; jB++)
	        {
	            jVB = ptrFace->getPtrElementRight()->convertToSingleIndex(jB, jV);
	            qReconstructionRight(jV) += ptrFace->basisFunction(Base::Side::RIGHT, jB, pRef) * solutionCoefficientsRight(jVB);
	        }
	    }


		if (qReconstructionLeft(0) < 0.0)
		{
			std::cout << "Element number: " << ptrFace->getID() << std::endl;
			std::cout << "Time: " << time << std::endl;
			std::cout << "density: " << qReconstructionLeft(0) << std::endl;
			std::cout << "momentum x-direction: " << qReconstructionLeft(1) << std::endl;
			std::cout << "momentum y-direction: " << qReconstructionLeft(2) << std::endl;
			std::cout << "density times energy: " << qReconstructionLeft(3) << std::endl;
			logger(ERROR,"ERROR: Negative energy left of the face");
		}

		if (qReconstructionRight(0) < 0.0)
		{
			std::cout << "Element number: " << ptrFace->getID() << std::endl;
			std::cout << "Time: " << time << std::endl;
			std::cout << "density: " << qReconstructionRight(0) << std::endl;
			std::cout << "momentum x-direction: " << qReconstructionRight(1) << std::endl;
			std::cout << "momentum y-direction: " << qReconstructionRight(2) << std::endl;
			std::cout << "density times energy: " << qReconstructionRight(3) << std::endl;
			logger(ERROR,"ERROR: Negative energy right of the face");
		}

	   // Compute normal vector, with size of the ref-to-phys face scale, pointing outward of the left element.
	   LinearAlgebra::NumericalVector normal = ptrFace->getNormalVector(pRef);

	   //Compute flux
	   LinearAlgebra::NumericalVector flux;
	   if (iSide == Base::Side::RIGHT)
	   {
		   normal *= -1;
		   flux = RoeRiemannFluxFunction(qReconstructionLeft, qReconstructionRight, normal, iSide);
	   }
	   else
	   {
		   flux = RoeRiemannFluxFunction(qReconstructionRight, qReconstructionLeft, normal, iSide);
	   }

	   // Compute integrand on the reference element.
	   std::size_t iVB; // Index for both variable and basis function.
	   for (std::size_t iB = 0; iB < numOfTestBasisFunctions; iB++)
	   {
	       for (std::size_t iV = 0; iV < numOfVariables_; iV++) // Index for direction
	       {
	           iVB = ptrFace->getPtrElement(iSide)->convertToSingleIndex(iB, iV);
	           integrand(iVB) = -flux(iV)*ptrFace->basisFunction(iSide, iB, pRef);
	       }
	   }

	   	if (integrand(0) != integrand(0))
	   	{
	   		std::cout << "The flux: " << -flux << std::endl;
	   		std::cout << "qReconstructionRight: " << qReconstructionRight << std::endl;
	   		std::cout << "qReconstructionLeft: " << qReconstructionLeft << std::endl;
	   		logger(ERROR,"ERROR: NaN found in internal face integrand.");
	   	}
	   return  integrand;
   }

   /// \brief Compute the right-hand side corresponding to a boundary face
   LinearAlgebra::NumericalVector Euler::computeRightHandSideAtFace(Base::Face *ptrFace, LinearAlgebra::NumericalVector &solutionCoefficients, const double time)
   {
	    // Define the integrand function for the right hand side for the reference face.
	    std::function<LinearAlgebra::NumericalVector(const Geometry::PointReference &)> integrandFunction = [&](const Geometry::PointReference &pRef) -> LinearAlgebra::NumericalVector
	    {   return this->integrandRightHandSideOnRefFace(ptrFace, time, pRef, solutionCoefficients);};

	    return faceIntegrator_.referenceFaceIntegral(ptrFace->getGaussQuadratureRule(), integrandFunction);
   }

   /// \brief Compute the right-hand side corresponding to an internal face
   LinearAlgebra::NumericalVector Euler::computeRightHandSideAtFace(Base::Face *ptrFace, const Base::Side side, LinearAlgebra::NumericalVector &solutionCoefficientsLeft, LinearAlgebra::NumericalVector &solutionCoefficientsRight, const double time)
   {
	    // Define the integrand function for the right hand side for the reference face.
	    std::function<LinearAlgebra::NumericalVector(const Geometry::PointReference &)> integrandFunction = [&](const Geometry::PointReference &pRef) -> LinearAlgebra::NumericalVector
	    {   return this->integrandRightHandSideOnRefFace(ptrFace, time, pRef, side, solutionCoefficientsLeft, solutionCoefficientsRight);};
	    std::cout << time << std::endl;
	    return faceIntegrator_.referenceFaceIntegral(ptrFace->getGaussQuadratureRule(), integrandFunction);
   }


















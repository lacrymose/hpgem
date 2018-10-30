/*
This file forms part of hpGEM. This package has been developed over a number of years by various people at the University of Twente and a full list of contributors can be found at
http://hpgem.org/about-the-code/team

This code is distributed using BSD 3-Clause License. A copy of which can found below.


Copyright (c) 2018, Univesity of Twenete
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "DGMaxDiscretization.h"

#include <complex>

#include "Base/HCurlConformingTransformation.h"
#include "Integration/ElementIntegral.h"
#include "Integration/FaceIntegral.h"

void DGMaxDiscretization::initializeBasisFunctions(Base::MeshManipulator<DIM> &mesh)
{
    mesh.useNedelecDGBasisFunctions();
    // TODO: This should probably also be exposed by using a constructor parameter.
    //mesh.useAinsworthCoyleDGBasisFunctions();
}

void DGMaxDiscretization::computeElementIntegrands(Base::MeshManipulator<DIM>& mesh, bool invertMassMatrix,
                                                   const InputFunction& sourceTerm,
                                                   const InputFunction& initialCondition,
                                                   const InputFunction& initialConditionDerivative) const
{
    LinearAlgebra::MiddleSizeMatrix massMatrix(1, 1), stiffnessMatrix(1, 1);
    LinearAlgebra::MiddleSizeVector initialConditionVector(1), initialConditionDerivativeVector(1), elementVector(1);
    Integration::ElementIntegral<DIM> elIntegral(false);

    elIntegral.setTransformation(std::shared_ptr<Base::CoordinateTransformation<DIM>> (new Base::HCurlConformingTransformation<DIM>()));
    for (hpGemUIExtentions::ElementIterator it = mesh.elementColBegin(); it != mesh.elementColEnd(); ++it)
    {
        std::size_t numberOfBasisFunctions = (*it)->getNumberOfBasisFunctions();

        //TODO: Are these resizes needed, as the content seems to be overwritten by the integral.
        massMatrix.resize(numberOfBasisFunctions, numberOfBasisFunctions);
        massMatrix = elIntegral.integrate<LinearAlgebra::MiddleSizeMatrix>((*it),
                [&](Base::PhysicalElement<DIM>& element) {
            LinearAlgebra::MiddleSizeMatrix res;
            elementMassMatrix(element, res);
            return res;
        });
        if(invertMassMatrix)
        {
            massMatrix = massMatrix.inverse();
        }
        (*it)->setElementMatrix(massMatrix, MASS_MATRIX_ID);

        stiffnessMatrix.resize(numberOfBasisFunctions, numberOfBasisFunctions);
        stiffnessMatrix = elIntegral.integrate<LinearAlgebra::MiddleSizeMatrix>((*it),
                [&](Base::PhysicalElement<DIM>& element) {
            LinearAlgebra::MiddleSizeMatrix res;
            elementStiffnessMatrix(element, res);
            return res;
        });
        (*it)->setElementMatrix(stiffnessMatrix, STIFFNESS_MATRIX_ID);

        // Note, resizes for vectors needed in case of empty functions
        initialConditionVector.resize(numberOfBasisFunctions);
        if (initialCondition)
        {
            initialConditionVector = elIntegral.integrate<LinearAlgebra::MiddleSizeVector>((*it),
                    [&](Base::PhysicalElement<DIM> &element) {
                LinearAlgebra::MiddleSizeVector res;
                elementInnerProduct(element,
                                    initialCondition,
                                    res); // Initial conditions
                return res;
            });
        }
        (*it)->setElementVector(initialConditionVector, INITIAL_CONDITION_VECTOR_ID);

        initialConditionDerivativeVector.resize(numberOfBasisFunctions);
        if (initialConditionDerivative)
        {
            initialConditionDerivativeVector = elIntegral.integrate<LinearAlgebra::MiddleSizeVector>((*it),
                    [&](Base::PhysicalElement<DIM> &element) {
                LinearAlgebra::MiddleSizeVector res;
                elementInnerProduct(element,
                                    initialConditionDerivative,
                                    res); // Initial conditions derivative
                return res;
            });
        }
        (*it)->setElementVector(initialConditionDerivativeVector, INITIAL_CONDITION_DERIVATIVE_VECTOR_ID);

        elementVector.resize(numberOfBasisFunctions);
        if (sourceTerm)
        {
            elementVector = elIntegral.integrate<LinearAlgebra::MiddleSizeVector>((*it),
                    [&](Base::PhysicalElement<DIM>& element) {
                LinearAlgebra::MiddleSizeVector res;
                elementInnerProduct(element, sourceTerm, res); // Source  term
                return res;
            });
        }


        (*it)->setElementVector(elementVector, SOURCE_TERM_VECTOR_ID);

    }
}

void DGMaxDiscretization::computeFaceIntegrals(
        Base::MeshManipulator<DIM>& mesh, const DGMaxDiscretization::FaceInputFunction& boundaryCondition,
        double stab) const
{
    LinearAlgebra::MiddleSizeMatrix stiffnessFaceMatrix(0, 0);
    LinearAlgebra::MiddleSizeVector boundaryFaceVector(1);
    Integration::FaceIntegral<DIM> faIntegral(false);

    faIntegral.setTransformation(std::shared_ptr<Base::CoordinateTransformation<DIM>> (new Base::HCurlConformingTransformation<DIM>()));
    for (hpGemUIExtentions::FaceIterator it = mesh.faceColBegin(); it != mesh.faceColEnd(); ++it)
    {

        // Resize all the matrices and vectors;
        std::size_t numberOfBasisFunctions = (*it)->getPtrElementLeft()->getNumberOfBasisFunctions();
        if ((*it)->isInternal())
        {
            numberOfBasisFunctions += (*it)->getPtrElementRight()->getNumberOfBasisFunctions();
        }

        stiffnessFaceMatrix.resize(numberOfBasisFunctions, numberOfBasisFunctions);
        boundaryFaceVector.resize(numberOfBasisFunctions);

        // Compute the actual face  integrals.
        stiffnessFaceMatrix = faIntegral.integrate<LinearAlgebra::MiddleSizeMatrix>((*it), [&](Base::PhysicalFace<DIM>& face) {
            LinearAlgebra::MiddleSizeMatrix res;
            faceMatrix(face, res);
            LinearAlgebra::MiddleSizeMatrix temp;
            facePenaltyMatrix(face, temp, stab);
            res += temp;
            return res;
        });
        (*it)->setFaceMatrix(stiffnessFaceMatrix, FACE_MATRIX_ID);

        if (boundaryCondition)
        {
            boundaryFaceVector = faIntegral.integrate<LinearAlgebra::MiddleSizeVector>((*it), [&](Base::PhysicalFace<DIM> &face) {
                LinearAlgebra::MiddleSizeVector res;
                faceVector(face, boundaryCondition, res, stab);
                return res;
            });
        }

        (*it)->setFaceVector(boundaryFaceVector, FACE_VECTOR_ID);
    }
}

void DGMaxDiscretization::elementMassMatrix(Base::PhysicalElement<DIM> &el, LinearAlgebra::MiddleSizeMatrix &ret) const
{
    const Base::Element* element = el.getElement();
    const std::size_t numberOfBasisFunctions = element->getNumberOfBasisFunctions();
    ret.resize(numberOfBasisFunctions, numberOfBasisFunctions);
    LinearAlgebra::SmallVector<DIM> phi_i, phi_j;
    double epsilon = static_cast<ElementInfos*>(element->getUserData())->epsilon_;
    for (std::size_t i = 0; i < numberOfBasisFunctions; ++i)
    {
        el.basisFunction(i, phi_i);
        for (std::size_t j = 0; j < numberOfBasisFunctions; ++j)
        {
            el.basisFunction(j, phi_j);
            ret(i, j) = phi_i * phi_j * epsilon;
            ret(j, i) = ret(i, j);
        }
    }
}

void DGMaxDiscretization::elementStiffnessMatrix(Base::PhysicalElement<DIM> &el,
                                                 LinearAlgebra::MiddleSizeMatrix &ret) const
{
    const Base::Element* element = el.getElement();
    const std::size_t  numberOfBasisFunctions = element->getNumberOfBasisFunctions();
    ret.resize(numberOfBasisFunctions, numberOfBasisFunctions);
    LinearAlgebra::SmallVector<DIM> phi_i, phi_j;
    for (std::size_t i = 0; i < numberOfBasisFunctions; ++i)
    {
        phi_i = el.basisFunctionCurl(i);
        for (std::size_t j = i; j < numberOfBasisFunctions; ++j)
        {
            phi_j = el.basisFunctionCurl(j);
            ret(i, j) = phi_i * phi_j;
            ret(j, i) = ret(i, j);
        }
    }
}

void DGMaxDiscretization::elementInnerProduct(Base::PhysicalElement<DIM> &el, const InputFunction &function,
                                              LinearAlgebra::MiddleSizeVector &ret) const
{
    const Base::Element* element = el.getElement();
    const Geometry::PointReference<DIM>& p = el.getPointReference();
    const std::size_t numberOfBasisFunctions = element->getNumberOfBasisFunctions();

    ret.resize(numberOfBasisFunctions);
    const PointPhysicalT pPhys = element->referenceToPhysical(p);
    LinearAlgebra::SmallVector<DIM> val, phi;
    function(pPhys, val);
    for (std::size_t i = 0; i < numberOfBasisFunctions; ++i)
    {
        el.basisFunction(i, phi);
        ret[i] = phi * val;
    }
}

void DGMaxDiscretization::faceMatrix(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeMatrix& ret) const
{
    const Base::Face* face = fa.getFace();

    std::size_t M = face->getPtrElementLeft()->getNrOfBasisFunctions();
    const bool internalFace = face->isInternal();
    if(internalFace)
    {
        M = face->getPtrElementLeft()->getNrOfBasisFunctions() + face->getPtrElementRight()->getNrOfBasisFunctions();
    }
    ret.resize(M, M);
    LinearAlgebra::SmallVector<DIM> phi_i_normal, phi_j_normal, phi_i_curl, phi_j_curl;
    for (std::size_t i = 0; i < M; ++i)
    {
        phi_i_curl = fa.basisFunctionCurl(i);
        fa.basisFunctionUnitNormalCross(i, phi_i_normal);

        for (std::size_t j = i; j < M; ++j)
        {
            phi_j_curl = fa.basisFunctionCurl(j);
            fa.basisFunctionUnitNormalCross(j, phi_j_normal);

            ret(i, j) = -(internalFace ? 0.5 : 1.) * (phi_i_normal * phi_j_curl + phi_j_normal * phi_i_curl);
            ret(j, i) = ret(i, j);
        }
    }
}

void DGMaxDiscretization::facePenaltyMatrix(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeMatrix& ret, double stab) const
{
    const Base::Face* face = fa.getFace();


    std::size_t M = face->getPtrElementLeft()->getNrOfBasisFunctions();
    if(face->isInternal())
    {
        M = face->getPtrElementLeft()->getNrOfBasisFunctions() + face->getPtrElementRight()->getNrOfBasisFunctions();
    }

    ret.resize(M, M);
    LinearAlgebra::SmallVector<DIM> phi_i, phi_j;
    for (std::size_t i = 0; i < M; ++i)
    {
        fa.basisFunctionUnitNormalCross(i, phi_i);
        for (std::size_t j = i; j < M; ++j)
        {
            fa.basisFunctionUnitNormalCross(j, phi_j);

            ret(i, j) = stab * (phi_i * phi_j);
            ret(j, i) = ret(i, j);
        }
    }

}

void DGMaxDiscretization::faceVector(Base::PhysicalFace<DIM>& fa, const FaceInputFunction& boundaryCondition, LinearAlgebra::MiddleSizeVector& ret, double stab) const
{
    const Base::Face* face = fa.getFace();
    LinearAlgebra::SmallVector<DIM> normal = fa.getNormalVector();
    normal /= Base::L2Norm(normal);
    const Geometry::PointReference<DIM - 1>& p = fa.getPointReference();

    if(face->isInternal())
    {
        std::size_t M = face->getPtrElementLeft()->getNrOfBasisFunctions() + face->getPtrElementRight()->getNrOfBasisFunctions();
        ret.resize(M);
        for(std::size_t i = 0 ; i < M; ++i)
            ret(i) = 0;
    }
    else
    {
        const ElementT* left = face->getPtrElementLeft();
        const Geometry::PointReference<DIM>& PLeft = face->mapRefFaceToRefElemL(p);

        PointPhysicalT PPhys;
        PPhys = left->referenceToPhysical(PLeft);
        LinearAlgebra::SmallVector<DIM> val, phi, phi_curl, boundaryValues;

        boundaryCondition(PPhys, fa, boundaryValues); //assumes the initial conditions and the boundary conditions match

        val = boundaryValues;
        std::size_t n = face->getPtrElementLeft()->getNrOfBasisFunctions();
        ret.resize(n);

        for (std::size_t i = 0; i < n; ++i)
        {
            fa.basisFunctionUnitNormalCross(i, phi);

            phi_curl = fa.basisFunctionCurl(i);

            ret(i) = -(phi_curl * val) + stab * (phi * val);
        }
    }

}

LinearAlgebra::SmallVector<DIM> DGMaxDiscretization::computeField(
        const Base::Element *element, const Geometry::PointReference<DIM> &p,
        const LinearAlgebra::MiddleSizeVector& coefficients) const
{
    logger.log(Log::WARN, "Only computing the real part of the field.");
    Base::PhysicalElement<DIM> physicalElement;
    physicalElement.setElement(element);
    std::shared_ptr<Base::CoordinateTransformation<DIM>> transform {new Base::HCurlConformingTransformation<DIM>()};
    physicalElement.setTransformation(transform);
    physicalElement.setPointReference(p);

    LinearAlgebra::SmallVector<DIM> result, phi;
    for (std::size_t i = 0; i < element->getNumberOfBasisFunctions(); ++i)
    {
        physicalElement.basisFunction(i, phi);
        result += std::real(coefficients[i]) * phi;
    }
    return result;
}

LinearAlgebra::SmallVector<DIM> DGMaxDiscretization::computeCurlField(
        const Base::Element *element, const Geometry::PointReference<DIM> &p,
        const LinearAlgebra::MiddleSizeVector &coefficients) const
{
    logger.log(Log::WARN, "Only computing the real part of the field.");
    Base::PhysicalElement<DIM> physicalElement;
    physicalElement.setElement(element);
    std::shared_ptr<Base::CoordinateTransformation<DIM>> transform {new Base::HCurlConformingTransformation<DIM>()};
    physicalElement.setTransformation(transform);
    physicalElement.setPointReference(p);

    LinearAlgebra::SmallVector<DIM> result;
    for (std::size_t i = 0; i< element->getNumberOfBasisFunctions(); ++i)
    {
        result += std::real(coefficients[i]) * physicalElement.basisFunctionCurl(i, 0);
    }
    return result;
}

// TODO: The code saves snapshots in the timeIntegrationVector, this is not particularly nice
// It might be better to pass the global vector here and distribute it ourselves.
std::map<DGMaxDiscretization::NormType, double> DGMaxDiscretization::computeError(
        Base::MeshManipulator<DIM>& mesh, std::size_t timeVector,
        DGMaxDiscretization::InputFunction electricField, DGMaxDiscretization::InputFunction electricFieldCurl,
        std::set<DGMaxDiscretization::NormType> norms) const
{
    // Note these are actually the squared norms
    double l2Norm = 0;
    double hCurlNorm = 0;
    // Setup the element integration.
    Integration::ElementIntegral<DIM> elIntegral(false);
    elIntegral.setTransformation(std::shared_ptr<Base::CoordinateTransformation<DIM>>(new Base::HCurlConformingTransformation<DIM>()));

    bool l2Wanted = norms.find(NormType::L2) != norms.end();
    bool hcurlWanted = norms.find(NormType::HCurl) != norms.end();
    bool dgWanted = norms.find(NormType::DG) != norms.end();

    for (Base::MeshManipulator<DIM>::ElementIterator it = mesh.elementColBegin(); it != mesh.elementColEnd(); ++it)
    {
        LinearAlgebra::SmallVector<2> errors = elIntegral.integrate<LinearAlgebra::SmallVector<2>>((*it),
                [&](Base::PhysicalElement<DIM>& el) {
            return elementErrorIntegrand(el, dgWanted || hcurlWanted, timeVector, electricField, electricFieldCurl);
        });
        l2Norm += errors[0];
        hCurlNorm += errors[1];
    }
    hCurlNorm += l2Norm;

    double dgNorm = hCurlNorm;

    if (dgWanted)
    {
        Integration::FaceIntegral<DIM> faIntegral(false);
        faIntegral.setTransformation(std::shared_ptr<Base::CoordinateTransformation<DIM> >(new Base::HCurlConformingTransformation<DIM>()));
        for (Base::MeshManipulator<DIM>::FaceIterator it = mesh.faceColBegin(); it != mesh.faceColEnd(); ++it)
        {
            dgNorm += faIntegral.integrate<double>(*it,
                    [&](Base::PhysicalFace<DIM>& face) {
                return faceErrorIntegrand(face, timeVector, electricField);
            });
        }
    }

    std::map<NormType, double> result;
    if (l2Wanted) result[L2] = sqrt(l2Norm);
    if (hcurlWanted) result[HCurl] = sqrt(hCurlNorm);
    if (dgWanted) result[DG] = sqrt(dgNorm);
    return result;
}

LinearAlgebra::SmallVector<2> DGMaxDiscretization::elementErrorIntegrand(
        Base::PhysicalElement<DIM> &el, bool computeCurl, std::size_t timeVector,
        DGMaxDiscretization::InputFunction exactValues, DGMaxDiscretization::InputFunction curlValues) const
{
    const Base::Element* element = el.getElement();
    const Geometry::PointReference<DIM>& p = el.getPointReference();
    PointPhysicalT pPhys;
    pPhys = element->referenceToPhysical(p);


    LinearAlgebra::SmallVector<DIM> phi, phiCurl, error, errorCurl;

    exactValues(pPhys, error);
    if (computeCurl)
    {
        curlValues(pPhys, errorCurl);
    }
    LinearAlgebra::MiddleSizeVector data;
    data = element->getTimeIntegrationVector(timeVector);
    for (std::size_t i = 0; i < element->getNrOfBasisFunctions(); ++i)
    {
        el.basisFunction(i, phi);
        error -= (std::real(data[i]) * phi);
        if (computeCurl)
        {
            phiCurl = el.basisFunctionCurl(i);
            errorCurl -= (std::real(data[i]) * phiCurl);
        }
    }
     double l2Error = Base::L2Norm(error);
    l2Error *= l2Error;
    double curlError = Base::L2Norm(errorCurl);
    curlError *= curlError;
    LinearAlgebra::SmallVector<2> errors;
    errors[0] = l2Error;
    errors[1] = curlError;
    return errors;
}

double DGMaxDiscretization::faceErrorIntegrand(Base::PhysicalFace<DIM> &fa, std::size_t timeVector,
                                               DGMaxDiscretization::InputFunction exactSolution) const
{
    // The face error part of the DG norm is given by
    // || h^0.5 [[u - u_h]]_T ||^2. Where h is the diameter of the face, u and
    // u_h are the exact and computed solutions. Further more the [[ . ]]_T is
    // the tangential jump, [[ a ]]_T = a_L x n_L + a_R x n_R for internal faces
    // and a x n for boundary faces (n is the normal).
    const Base::Face* face = fa.getFace();
    LinearAlgebra::SmallVector<DIM> normal = fa.getNormalVector();
    normal /= Base::L2Norm(normal);
    const Geometry::PointReference<2>& p = fa.getPointReference();

    ElementT* element = const_cast<ElementT*>(face->getPtrElementLeft());
    PointPhysicalT PPhys;
    const PointElementReferenceT& pElement = face->mapRefFaceToRefElemL(p);

    PPhys = element->referenceToPhysical(pElement);
    LinearAlgebra::SmallVector<DIM> error, phiNormal, solutionValues;

    // Compute u_L x n_L
    exactSolution(PPhys, solutionValues);
    error = normal.crossProduct(solutionValues);
    std::size_t n = face->getPtrElementLeft()->getNrOfBasisFunctions();
    LinearAlgebra::MiddleSizeVector solutionCoefficients = element->getTimeIntegrationVector(timeVector); //Issue regarding parallelisation is in this line....it goes out of bound for memory

    for (int i = 0; i < n; ++i)
    {
        // subtract the solution part, u_hL x n_L
        fa.basisFunctionUnitNormalCross(i, phiNormal);
        //TODO: What about the complex part of the solution.
        error -= std::real(solutionCoefficients[i]) * phiNormal;
    }
    if (face->isInternal())
    {
        // Note we reuse most of the vectors from the computation on the left side of the face.
        LinearAlgebra::SmallVector<DIM> otherSideError;
        element = const_cast<ElementT*>(face->getPtrElementRight());
        const PointElementReferenceT& pElement = face->mapRefFaceToRefElemR(p);
        PPhys = element->referenceToPhysical(pElement);
        // Compute u_R x n_L
        exactSolution(PPhys, solutionValues);
        otherSideError = normal.crossProduct(solutionValues);
        error -= otherSideError; // Note subtraction as n_L = - n_R.

        solutionCoefficients = element->getTimeIntegrationVector(timeVector);
        std::size_t M = face->getPtrElementLeft()->getNrOfBasisFunctions() + face->getPtrElementRight()->getNrOfBasisFunctions();

        for (std::size_t i = n; i < M; ++i)
        {
            // Subtract u_hR x n_R, note that the normal used internally is n_R
            // and not n_L as we have with the solution on the right, so again
            // subtraction.
            fa.basisFunctionUnitNormalCross(i, phiNormal);
            error -= (std::real(solutionCoefficients[i - n]) * phiNormal);
        }
    }
    double result = Base::L2Norm(error);
    result *= result;
    //To remove double contribution of flux computed on the boudary faces by different processors
    if(face->getFaceType() == Geometry::FaceType::SUBDOMAIN_BOUNDARY ||face->getFaceType() == Geometry::FaceType::PERIODIC_SUBDOMAIN_BC)
    {
        result /= 2;
    }
    return result;
}
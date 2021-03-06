/*
 This file forms part of hpGEM. This package has been developed over a number of
 years by various people at the University of Twente and a full list of
 contributors can be found at http://hpgem.org/about-the-code/team

 This code is distributed using BSD 3-Clause License. A copy of which can found
 below.


 Copyright (c) 2014, University of Twente
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ElementData.h"
#include "Logger.h"
#include "Element.h"

#include "LinearAlgebra/MiddleSizeMatrix.h"
#include "LinearAlgebra/MiddleSizeVector.h"
#include "UserData.h"
#include <functional>

namespace hpgem {

namespace Base {
ElementData::ElementData(std::size_t timeLevels, std::size_t numberOfUnknowns,
                         std::size_t numberOfElementMatrixes,
                         std::size_t numberOfElementVectors)
    : timeLevels_(timeLevels),
      numberOfUnknowns_(numberOfUnknowns),
      numberOfBasisFunctions_(numberOfUnknowns_),
      timeLevelDataVectors_(timeLevels_),
      userData_(nullptr),
      elementMatrix_(numberOfElementMatrixes),
      elementVector_(numberOfElementVectors) {
    logger(VERBOSE, "In constructor of ElementData: ");
    logger(VERBOSE, "numberOfElementMatrixes %", numberOfElementMatrixes);
    logger(VERBOSE, "elementMatrix_ size %", elementMatrix_.size());
    logger(VERBOSE, "numberOfElementVectors %", numberOfElementVectors);
    logger(VERBOSE, "elementVector_ size = %", elementVector_.size());
    // Setting numberOfBasisFunctions is left to the parent (Element)
}

ElementData::ElementData(const ElementData& other) {
    timeLevels_ = other.timeLevels_;
    numberOfUnknowns_ = other.numberOfUnknowns_;
    numberOfBasisFunctions_ = other.numberOfBasisFunctions_;

    timeLevelDataVectors_ = other.timeLevelDataVectors_;
    timeIntegrationVectors_ = other.timeIntegrationVectors_;

    // note: shallow copy
    userData_ = other.userData_;

    elementMatrix_ = other.elementMatrix_;
    elementVector_ = other.elementVector_;
}

void ElementData::setElementMatrix(
    const LinearAlgebra::MiddleSizeMatrix& matrix, std::size_t matrixID) {
    logger(VERBOSE, "In ElementData::setElementMatrix:");
    logger(VERBOSE, "matrix ID = %", matrixID);
    logger(VERBOSE, "elementMatrix_ size = %", elementMatrix_.size());
    if (matrixID >= elementMatrix_.size()) {
        logger(WARN,
               "Warning: Setting an element matrix that was not preallocated. "
               "If this is expected, please allocate more element matrixes in "
               "the mesh generator");
        elementMatrix_.resize(matrixID + 1);
    }
    elementMatrix_[matrixID] = matrix;
}

const LinearAlgebra::MiddleSizeMatrix& ElementData::getElementMatrix(
    std::size_t matrixID) const {
    logger.assert_debug(matrixID < elementMatrix_.size(),
                        "Requested matrix %, "
                        "while there are only % matrices for this element.",
                        matrixID, elementMatrix_.size());
    return elementMatrix_[matrixID];
}

LinearAlgebra::MiddleSizeMatrix& ElementData::getElementMatrix(
    std::size_t matrixID) {
    logger.assert_debug(matrixID < elementMatrix_.size(),
                        "Requested matrix %, "
                        "while there are only % matrices for this element.",
                        matrixID, elementMatrix_.size());
    return elementMatrix_[matrixID];
}

void ElementData::setElementVector(
    const LinearAlgebra::MiddleSizeVector& vector, std::size_t vectorID) {
    logger(VERBOSE, "In ElementData::setElementVector");
    logger(VERBOSE, "VectorID = %", vectorID);
    logger(VERBOSE, "elementVector size = %", elementVector_.size());

    if (vectorID >= elementVector_.size()) {
        logger(WARN,
               "Warning: Setting an element vector that was not "
               "preallocated. If this is expected, please allocate more "
               "element vectors in the mesh generator");
        elementVector_.resize(vectorID + 1);
    }
    elementVector_[vectorID] = vector;
}

LinearAlgebra::MiddleSizeVector ElementData::getElementVector(
    std::size_t vectorID) const {
    logger.assert_debug(vectorID < elementVector_.size(),
                        "insufficient element vectors stored");
    return elementVector_[vectorID];
}

void ElementData::setNumberOfBasisFunctions(std::size_t number) {
    for (std::size_t& index : numberOfBasisFunctions_) {
        index = number;
    }
    for (auto& vector : timeIntegrationVectors_) {
        vector.resize(getTotalNumberOfBasisFunctions());
    }
}

void ElementData::setNumberOfBasisFunctions(std::size_t number,
                                            std::size_t unknown) {
    numberOfBasisFunctions_[unknown] = number;
    for (auto& vector : timeIntegrationVectors_) {
        vector.resize(getTotalNumberOfBasisFunctions());
    }
}

std::size_t ElementData::getNrOfBasisFunctions() const {
    return getNumberOfBasisFunctions();
}

std::size_t ElementData::getNrOfBasisFunctions(std::size_t unknown) const {
    return getNumberOfBasisFunctions(unknown);
}

std::size_t ElementData::getNumberOfBasisFunctions() const {
    std::size_t number = numberOfBasisFunctions_[0];
    for (std::size_t index : numberOfBasisFunctions_)
        logger.assert_debug(
            index == number,
            "number of basis functions is different for different unknowns");
    return numberOfBasisFunctions_[0];
}

std::size_t ElementData::getNumberOfBasisFunctions(std::size_t unknown) const {
    logger.assert_debug(unknown < numberOfBasisFunctions_.size(),
                        "Asking for unknown % but there are only %", unknown,
                        numberOfBasisFunctions_.size());
    return numberOfBasisFunctions_[unknown];
}

std::size_t ElementData::getTotalNumberOfBasisFunctions() const {
    std::size_t number = 0;
    for (std::size_t index : numberOfBasisFunctions_) {
        number += index;
    }
    return number;
}

/// \param[in] timeLevel Index corresponding to the time level.
/// \param[in] unknown Index corresponding to the variable.
/// \param[in] basisFunction Index corresponding to the basisFunction.
/// \param[in] val Value to set the expansionCoeffient.
void ElementData::setData(std::size_t timeLevel, std::size_t unknown,
                          std::size_t basisFunction,
                          LinearAlgebra::MiddleSizeVector::type val) {
    logger.assert_debug(
        (timeLevel < timeLevels_ && unknown < numberOfUnknowns_ &&
         basisFunction < numberOfBasisFunctions_[unknown]),
        "Error: Asked for a time level, or unknown, greater than the amount of "
        "time levels");
    if (timeLevelDataVectors_[timeLevel].size() !=
        getTotalNumberOfBasisFunctions()) {
        timeLevelDataVectors_[timeLevel].resize(
            getTotalNumberOfBasisFunctions());
    }
    timeLevelDataVectors_[timeLevel](
        convertToSingleIndex(basisFunction, unknown)) = val;
}

/// \param[in] timeLevel Index corresponding to the time level.
/// \param[in] unknown Index corresponding to the variable.
/// \param[in] basisFunction Index corresponding to the basisFunction.
LinearAlgebra::MiddleSizeVector::type ElementData::getData(
    std::size_t timeLevel, std::size_t unknown,
    std::size_t basisFunction) const {
    logger.assert_debug(
        (timeLevel < timeLevels_ && unknown < numberOfUnknowns_ &&
         basisFunction < numberOfBasisFunctions_[unknown]),
        "Error: Asked for a time level, or unknown, greater than the amount of "
        "time levels");
    logger.assert_debug(timeLevelDataVectors_[timeLevel].size() ==
                            getTotalNumberOfBasisFunctions(),
                        "Wrong number of expansion coefficients.");
    return timeLevelDataVectors_[timeLevel](
        convertToSingleIndex(basisFunction, unknown));
}

/// \details Rewrite with swap!!! This method is slow and inefficient. It is
/// therefore advised to use getTimeLevelDataVector instead. \param[in]
/// timeLevel Index corresponding to the time level. \param[in] unknown Index
/// corresponding to the variable. \param[in] val Vector of values to set the
/// expansionCoeffient corresponding to the given unknown and time level.
void ElementData::setTimeLevelData(std::size_t timeLevel, std::size_t unknown,
                                   const LinearAlgebra::MiddleSizeVector& val) {
    logger.assert_debug(val.size() == numberOfBasisFunctions_[unknown],
                        "data vector has the wrong size");
    logger.assert_debug(
        (timeLevel < timeLevels_ && unknown < numberOfUnknowns_),
        "Error: Asked for a time level, or unknown, greater than the amount of "
        "time levels");
    if (timeLevelDataVectors_[timeLevel].size() !=
        getTotalNumberOfBasisFunctions()) {
        timeLevelDataVectors_[timeLevel].resize(
            getTotalNumberOfBasisFunctions());
    }

    for (std::size_t iB = 0; iB < val.size(); ++iB)  // iB = iBasisFunction
    {
        timeLevelDataVectors_[timeLevel](convertToSingleIndex(iB, unknown)) =
            val[iB];
    }
}

void ElementData::setTimeLevelData(std::size_t timeLevel,
                                   const LinearAlgebra::MiddleSizeVector& val) {
    std::size_t number = numberOfBasisFunctions_[0];
    for (std::size_t index : numberOfBasisFunctions_)
        logger.assert_debug(
            index == number,
            "number of basis functions is different for different unknown");
    logger.assert_debug(val.size() == number, "data vector has the wrong size");
    logger.assert_debug(
        timeLevel < timeLevels_,
        "Asked for time level %, but there are only % time levels", timeLevel,
        timeLevels_);
    setTimeLevelData(timeLevel, 0, val);
}

/// \param[in] timeLevel Index corresponding to the time level.
/// \param[in] unknown Index corresponding to the variable.
const LinearAlgebra::MiddleSizeVector ElementData::getTimeLevelData(
    std::size_t timeLevel, std::size_t unknown) const {
    logger.assert_debug(
        timeLevel < timeLevels_,
        "Asked for time level %, but there are only % time levels", timeLevel,
        timeLevels_);

    LinearAlgebra::MiddleSizeVector timeLevelData(
        numberOfBasisFunctions_[unknown]);
    for (std::size_t iB = 0; iB < numberOfBasisFunctions_[unknown];
         iB++)  // iB = iBasisFunction
    {
        timeLevelData(iB) =
            timeLevelDataVectors_[timeLevel](convertToSingleIndex(iB, unknown));
    }
    return timeLevelData;
}

/// \param[in] timeLevel Index corresponding to the time level.
/// \param[in] val Vector of values to set the expansionCoeffient corresponding
/// to the given unknown and time level.
void ElementData::setTimeLevelDataVector(std::size_t timeLevel,
                                         LinearAlgebra::MiddleSizeVector& val) {
    logger.assert_debug(val.size() == getTotalNumberOfBasisFunctions(),
                        "data vector has the wrong size");
    logger.assert_debug(timeLevel < timeLevels_,
                        "Asked for time level %, but there"
                        " are only % time levels",
                        timeLevel, timeLevels_);
    // The vector can be of dimension 0 if it hasn't been used before,
    // therefore it must be resized first.
    if (timeLevelDataVectors_[timeLevel].size() !=
        getTotalNumberOfBasisFunctions()) {
        timeLevelDataVectors_[timeLevel].resize(
            getTotalNumberOfBasisFunctions());
    }
    timeLevelDataVectors_[timeLevel] = val;
}

/// \param[in] timeLevel Index corresponding to the time level.
/// \return The expansion coefficient corresponding to the given time level.
const LinearAlgebra::MiddleSizeVector& ElementData::getTimeLevelDataVector(
    std::size_t timeLevel) const {
    logger.assert_debug(
        timeLevel < timeLevels_,
        "Asked for time level %, but there are only % time levels", timeLevel,
        timeLevels_);
    logger.assert_debug(timeLevelDataVectors_[timeLevel].size() ==
                            getTotalNumberOfBasisFunctions(),
                        "Wrong number of expansion coefficients.");
    return timeLevelDataVectors_[timeLevel];
}

LinearAlgebra::MiddleSizeVector& ElementData::getTimeLevelDataVector(
    std::size_t timeLevel) {
    logger.assert_debug(
        timeLevel < timeLevels_,
        "Asked for time level %, but there are only % time levels", timeLevel,
        timeLevels_);
    // The vector can be of dimension 0 if it hasn't been used before,
    // therefore it must be resized first.
    if (timeLevelDataVectors_[timeLevel].size() !=
        getTotalNumberOfBasisFunctions()) {
        timeLevelDataVectors_[timeLevel].resize(
            getTotalNumberOfBasisFunctions());
    }
    return timeLevelDataVectors_[timeLevel];
}

const LinearAlgebra::MiddleSizeVector& ElementData::getTimeIntegrationVector(
    std::size_t timeIntegrationVectorId) const {
    logger.assert_debug(
        timeIntegrationVectorId < timeIntegrationVectors_.size(),
        "Asked for time integration vector %, but there are only % time "
        "integration vectors",
        timeIntegrationVectorId, timeIntegrationVectors_.size());
    logger.assert_debug(
        timeIntegrationVectors_[timeIntegrationVectorId].size() ==
            getTotalNumberOfBasisFunctions(),
        "Size of time integration vector is %, but should be %.",
        timeIntegrationVectors_[timeIntegrationVectorId].size(),
        getTotalNumberOfBasisFunctions());
    return timeIntegrationVectors_[timeIntegrationVectorId];
}

LinearAlgebra::MiddleSizeVector& ElementData::getTimeIntegrationVector(
    std::size_t timeIntegrationVectorId) {
    logger.assert_debug(
        timeIntegrationVectorId < timeIntegrationVectors_.size(),
        "Asked for time integration vector %, but there are only % time "
        "integration vectors",
        timeIntegrationVectorId, timeIntegrationVectors_.size());
    logger.assert_debug(
        timeIntegrationVectors_[timeIntegrationVectorId].size() ==
            getTotalNumberOfBasisFunctions(),
        "Size of time integration vector is %, but should be %.",
        timeIntegrationVectors_[timeIntegrationVectorId].size(),
        getTotalNumberOfBasisFunctions());
    return timeIntegrationVectors_[timeIntegrationVectorId];
}

void ElementData::setTimeIntegrationVector(
    std::size_t timeIntegrationVectorId, LinearAlgebra::MiddleSizeVector& val) {
    logger.assert_debug(
        timeIntegrationVectorId < timeIntegrationVectors_.size(),
        "Asked for time integration vector %, but there are only % time "
        "integration vectors",
        timeIntegrationVectorId, timeIntegrationVectors_.size());
    logger.assert_debug(val.size() == getTotalNumberOfBasisFunctions(),
                        "Size of the vector with which to set the time "
                        "integration vector is %, but should be %",
                        val.size(), getTotalNumberOfBasisFunctions());

    // The vector can be of dimension 0 if it hasn't been used before,
    // therefore it must be resized first.
    if (timeIntegrationVectors_[timeIntegrationVectorId].size() !=
        getTotalNumberOfBasisFunctions()) {
        timeIntegrationVectors_[timeIntegrationVectorId].resize(
            getTotalNumberOfBasisFunctions());
    }
    timeIntegrationVectors_[timeIntegrationVectorId] = val;
}

/// \details Return a vector of size numberOfBasisFunctions_ that corresponds to
/// the given unknown (variable id). Let u be the complete vector corresponding
/// to the timeIntegrationVectorId and let u_iV be the subvector that is
/// returned, where iV = unknown. Then u_iV satisfies u_iV(iB) = u(iVB) where
/// iVB = convertToSingleIndex(iB,iV) for all 0 <= iB < numberOfBasisFunctions_.
const LinearAlgebra::MiddleSizeVector ElementData::getTimeIntegrationSubvector(
    std::size_t timeIntegrationVectorId, std::size_t unknown) const {
    logger.assert_debug(unknown < numberOfUnknowns_,
                        "Asking for unknown % but there are only %", unknown,
                        numberOfUnknowns_);
    logger.assert_debug(
        timeIntegrationVectorId < timeIntegrationVectors_.size(),
        "Asked for time integration vector %, but there are only % time "
        "integration vectors",
        timeIntegrationVectorId, timeIntegrationVectors_.size());
    logger.assert_debug(
        timeIntegrationVectors_[timeIntegrationVectorId].size() ==
            getTotalNumberOfBasisFunctions(),
        "Size of time integration vector is %, but should be %.",
        timeIntegrationVectors_[timeIntegrationVectorId].size(),
        getTotalNumberOfBasisFunctions());

    LinearAlgebra::MiddleSizeVector timeIntegrationSubvector(
        numberOfBasisFunctions_[unknown]);
    for (std::size_t iB = 0; iB < numberOfBasisFunctions_[unknown];
         iB++)  // iB = iBasisFunction
    {
        timeIntegrationSubvector(iB) =
            timeIntegrationVectors_[timeIntegrationVectorId](
                convertToSingleIndex(iB, unknown));
    }
    return timeIntegrationSubvector;
}

/// \details Set part of the vector that corresponds to the given
/// timeIntegrationVectorId and unknown (variable id). Let u be the complete
/// vector corresponding to the timeIntegrationVectorId and let v (= val) be the
/// vector of size numberOfBasisFunctions_ with which to set the subvector u_iV
/// of u, where iV = unknown. Then at the end u will satisfy u(iVB) = v(iB)
/// where iVB = convertToSingleIndex(iB,iV) for all 0 <= iB <
/// numberOfBasisFunctions_.
void ElementData::setTimeIntegrationSubvector(
    std::size_t timeIntegrationVectorId, std::size_t unknown,
    LinearAlgebra::MiddleSizeVector val) {
    logger.assert_debug(
        timeIntegrationVectorId < timeIntegrationVectors_.size(),
        "Asked for time integration vector %, but there are only % time "
        "integration vectors",
        timeIntegrationVectorId, timeIntegrationVectors_.size());
    logger.assert_debug(val.size() == numberOfBasisFunctions_[unknown],
                        "Size of the vector with which to set a subvector of "
                        "the time integration vector is %, but should be %",
                        val.size(), numberOfBasisFunctions_[unknown]);

    // The vector can be of dimension 0 if it hasn't been used before,
    // therefore it must be resized first.
    if (timeIntegrationVectors_[timeIntegrationVectorId].size() !=
        getTotalNumberOfBasisFunctions()) {
        timeIntegrationVectors_[timeIntegrationVectorId].resize(
            getTotalNumberOfBasisFunctions());
    }
    for (std::size_t iB = 0; iB < numberOfBasisFunctions_[unknown];
         iB++)  // iB = iBasisFunction
    {
        timeIntegrationVectors_[timeIntegrationVectorId](
            convertToSingleIndex(iB, unknown)) = val(iB);
    }
}

LinearAlgebra::MiddleSizeVector::type ElementData::getTimeIntegrationData(
    std::size_t timeIntegrationVectorId, std::size_t unknown,
    std::size_t basisFunction) const {
    logger.assert_debug(
        timeIntegrationVectorId < timeIntegrationVectors_.size(),
        "Asked for time integration vector %, but there are only % time "
        "integration vectors",
        timeIntegrationVectorId, timeIntegrationVectors_.size());
    logger.assert_debug(
        timeIntegrationVectors_[timeIntegrationVectorId].size() ==
            getTotalNumberOfBasisFunctions(),
        "Size of time integration vector is %, but should be %.",
        timeIntegrationVectors_[timeIntegrationVectorId].size(),
        getTotalNumberOfBasisFunctions());

    return timeIntegrationVectors_[timeIntegrationVectorId](
        convertToSingleIndex(basisFunction, unknown));
}

void ElementData::setTimeIntegrationData(
    std::size_t timeIntegrationVectorId, std::size_t unknown,
    std::size_t basisFunction, LinearAlgebra::MiddleSizeVector::type val) {
    logger.assert_debug(
        timeIntegrationVectorId < timeIntegrationVectors_.size(),
        "Asked for time integration vector %, but there are only % time "
        "integration vectors",
        timeIntegrationVectorId, timeIntegrationVectors_.size());

    // The vector can be of dimension 0 if it hasn't been used before,
    // therefore it must be resized first.
    if (timeIntegrationVectors_[timeIntegrationVectorId].size() !=
        getTotalNumberOfBasisFunctions()) {
        timeIntegrationVectors_[timeIntegrationVectorId].resize(
            getTotalNumberOfBasisFunctions());
    }
    timeIntegrationVectors_[timeIntegrationVectorId](
        convertToSingleIndex(basisFunction, unknown)) = val;
}

std::size_t ElementData::getNrOfUnknows() const {
    return getNumberOfUnknowns();
}

std::size_t ElementData::getNrOfUnknowns() const {
    return getNumberOfUnknowns();
}

std::size_t ElementData::getNumberOfUnknowns() const {
    return numberOfUnknowns_;
}

void ElementData::setUserData(UserElementData* data) const {
    // the user may pass any kind of data he/she wants (including nullptr) even
    // if this does not seem to make sense
    userData_ = data;
}

UserElementData* ElementData::getUserData() const { return userData_; }
}  // namespace Base

}  // namespace hpgem

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


#ifndef SHORTTERMSTORAGEELEMENT_HPP_
#define SHORTTERMSTORAGEELEMENT_HPP_

#include "Base/Element.hpp"

///\BUG resolves field has incomplete type
#include "Geometry/PointReference.hpp"
#include "Geometry/Jacobian.hpp"
#include "Node.hpp"
#include <cassert>

namespace Base
{

    class Element;

    /**
     * An element that computes and stores all basis function values and derivatives.
     * This way user can just type ret(j,i)=basisFunctionDeriv(i,p)*basisFunctionDeriv(j,p)
     * without being bothered by unnecessary information about Jacobians or transformations
     * and without having to compute the Jacobian twice for every entry in the element matrix
     *
     * This class will automatically recompute all data whenever a new point is passed to a basisfunction
     *
     * This cannot be directly implemented in Element because then all Element will store all this data
     * resulting in a massive storage overhead
     *
     * The actual transformations required depend on the function space you are working in, so the actual work
     * is delegated to a subclass that doesn't need to shield all the getters.
     *
     * You cannot use this class to modify elements
     *
     * Be VERY careful to not put this type of element in a mesh, the extra storage needed for this type of elements will likely crash your program
     * Once proper error checking/handling is implemented safeguards will be added to make this a bit more difficult
     */
    class ShortTermStorageElementBase : public Element
    {
    public:

        ShortTermStorageElementBase(unsigned int dimension, bool useCache = false) :
        Element(), //the superclass is not meant for actual use
        element_(nullptr), //I dont like that face_ is not defined before operator= is called at least once
        currentPoint_(dimension), //but I want to give users the ability to pass alternative wrappers to the integrators
        jac_(dimension, dimension), //without forcing them to pick a random face that is going to be discarded anyway
        recomputeCache_(true),
        currentPointIndex_(-1),
        useCache_(useCache) { }

        ///recomputes the jacobian, the physical point, functionvalues and derivatives of functions based on the current point
        virtual void computeData();

        Element& operator=(const Element& element)
        {//todo check that &element and this are different things (errorChecker)
            element_ = &element;
            /// \bug This should go back to NAN at some point. Again to fix problems with math and STL::vector
            ///\bug placing 0/0 here breaks one of the PETSc based self tests
            currentPoint_[0] = 1. / 0.;
            currentPointIndex_ = -1;
            return *this;
        }

        ShortTermStorageElementBase(const ShortTermStorageElementBase& copy) : element_(copy.element_), currentPoint_(copy.currentPoint_), jac_(copy.jac_), useCache_(copy.useCache_), currentPointIndex_(copy.currentPointIndex_), recomputeCache_(copy.recomputeCache_) { }

        ~ShortTermStorageElementBase() {
            //keep the element alive!
        }

        virtual double basisFunction(unsigned int i, const PointReferenceT& p)
        {
            throw "No storage functionality was implemented! Are you working in a vector valued function space?";
        }

        virtual void basisFunction(unsigned int i, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret)
        {
            throw "No storage functionality was implemented! Are you working in a scalar function space?";
        }

        double basisFunctionDeriv(unsigned int i, unsigned int jDir, const PointReferenceT& p) const override
        {
            return element_->basisFunctionDeriv(i, jDir, p);
        }

        virtual void basisFunctionDeriv(unsigned int i, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret, const Element* = NULL)
        {
            throw "No storage functionality was implemented! Did you mean basisFunctionCurl?";
        }

        virtual void basisFunctionCurl(unsigned int i, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret)
        {
            throw "No storage functionality was implemented! Did you mean basisFunctionDeriv?";
        }

        double basisFunction(unsigned int i, const PointReferenceT& p) const override
        {
            throw "No storage functionality was implemented! Are you working in a vector valued function space?";
        }

        void basisFunction(unsigned int i, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret) const override
        {
            throw "No storage functionality was implemented! Are you working in a scalar function space?";
        }

        void basisFunctionDeriv(unsigned int i, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret, const Element* = NULL) const override
        {
            throw "No storage functionality was implemented! Did you mean basisFunctionCurl?";
        }

        void basisFunctionCurl(unsigned int i, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret) const override
        {
            throw "No storage functionality was implemented! Did you mean basisFunctionDeriv?";
        }

        virtual void calcJacobian(const PointReferenceT& pointReference, JacobianT& jacobian);

        void calcJacobian(const PointReferenceT& pointReference, JacobianT& jacobian) const override;

        //if this is needed a lot, also store this
        void referenceToPhysical(const PointReferenceT& pointReference, PointPhysicalT& pointPhysical)const override;

        //caching functionality

        //! \brief Start caching (geometry) information now.
        void cacheOn();

        //! \brief Stop using cache.
        void cacheOff();

        //! \brief Set recompute the cache ON.
        void recomputeCacheOn();

        //! \brief Set recompute the cache OFF.
        void recomputeCacheOff();

        //make sure all the other function map to the current element

        unsigned int getID()const override
        {
            return element_->getID();
        }

        unsigned int getID() override
        {
            return element_->getID();
        }

        const GaussQuadratureRuleT* getGaussQuadratureRule() const override
        {
            return element_->getGaussQuadratureRule();
        }

        void getSolution(unsigned int timeLevel, const PointReferenceT& p, SolutionVector& solution) const override
        {
            return element_->getSolution(timeLevel, p, solution);
        }

        int getLocalNrOfBasisFunctions() const override
        {
            return element_->getLocalNrOfBasisFunctions();
        }

        const Face* getFace(int localFaceNr)const override
        {
            return element_->getFace(localFaceNr);
        }

        const Edge* getEdge(int localEdgeNr)const override
        {
            return element_->getEdge(localEdgeNr);
        }

        const Node* getNode(int localNodeNr)const override
        {
            return element_->getNode(localNodeNr);
        }

        int getNrOfFaces() const override
        {
            return element_->getNrOfFaces();
        }

        int getNrOfEdges() const override
        {
            return element_->getNrOfEdges();
        }

        unsigned int getNrOfNodes() const override
        {
            return element_->getNrOfNodes();
        }

#ifndef NDEBUG

        const Base::BaseBasisFunction* getBasisFunction(int i)const override
        {
            return element_->getBasisFunction(i);
        }
#endif

        void getElementMatrix(LinearAlgebra::Matrix& mat, int matrixID = 0) const override
        {
            element_->getElementMatrix(mat, matrixID);
        }

        void getElementVector(LinearAlgebra::NumericalVector& vec, int vectorID = 0) const override
        {
            element_->getElementVector(vec, vectorID);
        }

        const LinearAlgebra::NumericalVector getTimeLevelData(std::size_t timeLevel, std::size_t unknown = 0) const override
        {
            return element_->getTimeLevelData(timeLevel);
        }

        double getData(unsigned int timeLevel, unsigned int unknown, unsigned int basisFunction) const override
        {
            return element_->getData(timeLevel, unknown, basisFunction);
        }

        int getNrOfUnknows() const override
        {
            return element_->getNrOfUnknows();
        }

        int getNrOfBasisFunctions() const override
        {
            return element_->getNrOfBasisFunctions();
        }

        const LinearAlgebra::NumericalVector& getResidue() const override
        {
            return element_->getResidue();
        }

        UserElementData* getUserData() const override
        {
            return element_->getUserData();
        }

        const MappingReferenceToPhysicalT * const getReferenceToPhysicalMap() const override
        {
            return element_->getReferenceToPhysicalMap();
        }

        const PhysicalGeometryT * const getPhysicalGeometry() const override
        {
            return element_->getPhysicalGeometry();
        }

        const ReferenceGeometryT * const getReferenceGeometry() const override
        {
            return element_->getReferenceGeometry();
        }

        const RefinementGeometryT* getRefinementGeometry() const override
        {
            return element_->getRefinementGeometry();
        }

    private:

        ShortTermStorageElementBase& operator=(const ShortTermStorageElementBase&)
        {
            throw "you are already storing the data, no need to store it twice!";
        }

    protected:
        const Element* element_;
        Geometry::PointReference currentPoint_;

        Geometry::Jacobian jac_;

        std::vector<LinearAlgebra::NumericalVector> basisFunctionValues_, basisFunctionDerivatives_;
    private:

        bool useCache_;
        bool recomputeCache_;
        int currentPointIndex_;
    };
}



#endif /* SHORTTERMSTORAGEELEMENT_HPP_ */

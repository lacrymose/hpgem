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

#include "Element.h"
#include "PhysGradientOfBasisFunction.h"
#include "Edge.h"
#include "FaceCacheData.h"
#include "Face.h"

#include "BasisFunctionSet.h"
#include "Geometry/PhysicalGeometry.h"
#include "LinearAlgebra/MiddleSizeVector.h"
#include "Geometry/PointPhysical.h"
#include "ElementCacheData.h"
#include "Geometry/ReferenceGeometry.h"
#include "Geometry/PointReference.h"
#include "Logger.h"
#include "Node.h"
#include "Integration/QuadratureRules/GaussQuadratureRule.h"
#include "Geometry/Jacobian.h"

#include <limits>
#include <algorithm>

namespace Base
{
        
    Element::Element(bool owned, const ElementData& otherData, const Geometry::ElementGeometry& otherGeometry)
        : owned_ (owned),
        ElementGeometry(otherGeometry), ElementData(otherData)
    {        
    }
    
    Element* Element::copyWithoutFacesEdgesNodes()
    {
        //Make a new element with the data and geometry of this element
        Element* newElement = new Element(this->owned_, *this, *this);
        
        //copy the pointers to singletons
        newElement->quadratureRule_ = quadratureRule_;
        newElement->basisFunctionSet_ = basisFunctionSet_;
        
        //copy other data
        newElement->basisFunctionSetPositions_ = basisFunctionSetPositions_;
        newElement->id_ = id_;
        newElement->numberOfDOFinTheElement_ = numberOfDOFinTheElement_;
        newElement->orderCoeff_ = orderCoeff_;
        newElement->vecCacheData_ = vecCacheData_;
        
        //allocate memory for nodesList, facesList and edgesList
        newElement->nodesList_.resize(getNumberOfNodes());
        newElement->edgesList_.resize(getNumberOfEdges());
        newElement->facesList_.resize(getNumberOfFaces());
        
        return newElement;
    }

    void Element::setDefaultBasisFunctionSet(std::size_t position, std::size_t unknown)
    {
        logger.assert_debug(unknown < getNumberOfUnknowns(), "Setting unknown % but there are only %",
                            unknown, getNumberOfUnknowns());
        logger.assert_debug(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        logger.assert_debug(unknown < getNumberOfUnknowns(),
                            "Setting basis function for unknown % with number of unknowns %", unknown, getNumberOfUnknowns());
        basisFunctionSetPositions_[unknown].resize(1, -1);
        basisFunctionSetPositions_[unknown][0] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_[unknown])
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions, unknown);
        setQuadratureRulesWithOrder(orderCoeff_ * basisFunctionSet_->at(position)->getOrder() + 1);
        numberOfDOFinTheElement_[unknown] = basisFunctionSet_->at(position)->size();
    }
    
    void Element::setFaceBasisFunctionSet(std::size_t position, std::size_t localFaceIndex, std::size_t unknown)
    {
        logger.assert_debug(unknown < getNumberOfUnknowns(), "Setting unknown % but there are only %",
                            unknown, getNumberOfUnknowns());
        logger.assert_debug(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        logger.assert_debug(unknown < getNumberOfUnknowns(),
                            "Setting basis function for unknown % with number of unknowns %", unknown, getNumberOfUnknowns());
        logger.assert_debug(localFaceIndex < getNumberOfFaces(), "Asked for face %, but there are only % faces", localFaceIndex, getNumberOfFaces());
        if (basisFunctionSetPositions_[unknown].size() < 1 + getNumberOfFaces())
        {
            basisFunctionSetPositions_[unknown].resize(1 + getNumberOfFaces(), -1);
        }
        basisFunctionSetPositions_[unknown][1 + localFaceIndex] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_[unknown])
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions, unknown);
    }
    
    void Element::setEdgeBasisFunctionSet(std::size_t position, std::size_t localEdgeIndex, std::size_t unknown)
    {
        logger.assert_debug(unknown < getNumberOfUnknowns(), "Setting unknown % but there are only %",
                            unknown, getNumberOfUnknowns());
        logger.assert_debug(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        logger.assert_debug(unknown < getNumberOfUnknowns(),
                            "Setting basis function for unknown % with number of unknowns %", unknown, getNumberOfUnknowns());
        logger.assert_debug(localEdgeIndex < getNumberOfEdges(), "Asked for edge %, but there are only % edges", localEdgeIndex, getNumberOfEdges());
        if (basisFunctionSetPositions_[unknown].size() < 1 + getNumberOfFaces() + getNumberOfEdges())
        {
            basisFunctionSetPositions_[unknown].resize(1 + getNumberOfFaces() + getNumberOfEdges(), -1);
        }
        basisFunctionSetPositions_[unknown][1 + getNumberOfFaces() + localEdgeIndex] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_[unknown])
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions, unknown);
    }
    
    void Element::setVertexBasisFunctionSet(std::size_t position, std::size_t localNodeIndex, std::size_t unknown)
    {
        logger.assert_debug(unknown < getNumberOfUnknowns(), "Setting unknown % but there are only %",
                            unknown, getNumberOfUnknowns());
        logger.assert_debug(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        logger.assert_debug(unknown < getNumberOfUnknowns(),
                            "Setting basis function for unknown % with number of unknowns %", unknown, getNumberOfUnknowns());
        logger.assert_debug(localNodeIndex < getNumberOfNodes(), "Asked for node %, but there are only % nodes", localNodeIndex, getNumberOfNodes());
        if (basisFunctionSetPositions_[unknown].size() < 1 + getNumberOfFaces() + getNumberOfEdges() + getNumberOfNodes())
        {
            basisFunctionSetPositions_[unknown].resize(1 + getNumberOfFaces() + getNumberOfEdges() + getNumberOfNodes(), -1);
        }
        basisFunctionSetPositions_[unknown][1 + getNumberOfFaces() + getNumberOfEdges() + localNodeIndex] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_[unknown])
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions, unknown);
    }
    
    std::size_t Element::getID() const
    {
        return id_;
    }
    
    std::size_t Element::getID()
    {
        return id_;
    }
    
    void Element::setQuadratureRulesWithOrder(std::size_t quadrROrder)
    {
        if (quadratureRule_ == nullptr)
        {
            quadratureRule_ = Geometry::ElementGeometry::referenceGeometry_->getGaussQuadratureRule(quadrROrder);
        }
        else if(quadrROrder > quadratureRule_->order())
        {
            quadratureRule_ = Geometry::ElementGeometry::referenceGeometry_->getGaussQuadratureRule(quadrROrder);
        }
    }
    
    void Element::setGaussQuadratureRule(QuadratureRules::GaussQuadratureRule* const quadR)
    {
        logger.assert_debug(quadR != nullptr, "Invalid quadrature rule passed");
        quadratureRule_ = quadR;
    }
    
    QuadratureRules::GaussQuadratureRule* Element::getGaussQuadratureRule() const
    {
        return quadratureRule_;
    }
    
    std::vector<Base::ElementCacheData>& Element::getVecCacheData()
    {
        return vecCacheData_;
    }
    
#ifndef NDEBUG
    const Base::BaseBasisFunction* Element::getBasisFunction(std::size_t i) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(), "Asked for basis function %, but there are only % basis functions", i, getNumberOfBasisFunctions());
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i);
        return (*subSet)[subIndex];
    }
#endif
    
    void Element::setFace(std::size_t localFaceNumber, Face* face)
    {
        logger.assert_debug(localFaceNumber < getNumberOfFaces(), "Asked for face %, but there are only % faces", localFaceNumber, getNumberOfFaces());
        logger.assert_debug(face != nullptr, "Invalid face passed");
        logger.assert_debug((face->getPtrElementLeft() == this && face->localFaceNumberLeft() == localFaceNumber)
                            || (face->getPtrElementRight() == this && face->localFaceNumberRight() == localFaceNumber),
                            "You are only allowed to set a face to a local face index that matches");
        if (facesList_.size() < localFaceNumber + 1)
        {
            logger(WARN, "Resizing the facesList, since it's smaller(%) than to localFaceNumber + 1(%)", facesList_.size(), localFaceNumber + 1);
            facesList_.resize(localFaceNumber + 1);
        }
        facesList_[localFaceNumber] = face;
    }
    
    void Element::setEdge(std::size_t localEdgeNumber, Edge* edge)
    {
        logger.assert_debug(localEdgeNumber < getNumberOfEdges(), "Asked for edge %, but there are only % edges", localEdgeNumber, getNumberOfEdges());
        logger.assert_debug(edge != nullptr, "Invalid edge passed");
        //This if-statement is needed, since it could happen in 4D
        if (edgesList_.size() < localEdgeNumber + 1)
        {
            edgesList_.resize(localEdgeNumber + 1);
        }
        edgesList_[localEdgeNumber] = edge;
    }
    
    void Element::setNode(std::size_t localNodeNumber, Node* node)
    {
        logger.assert_debug(node != nullptr, "Invalid node passed");
        logger.assert_debug(std::find(nodesList_.begin(), nodesList_.end(), node) == nodesList_.end(), "Trying to add node %, but it was already added",
                            node->getID());
        logger.assert_debug(localNodeNumber < getNumberOfNodes(), "Asked for node %, but there are only % nodes", localNodeNumber, getNumberOfNodes());
        nodesList_[localNodeNumber] = node;
    }

    std::ostream& operator<<(std::ostream& os, const Element& element)
    {
        os << '(';
        const Geometry::ElementGeometry& elemG = static_cast<const Geometry::ElementGeometry&>(element);
        operator<<(os, elemG);
        os << std::endl;
        return os;
    }

    std::tuple<const BasisFunctionSet*, std::size_t> Element::getBasisFunctionSetAndIndex(size_t index) const
    {
        logger.assert_debug(index < getNumberOfBasisFunctions(), "Asked for basis function %, but there are only % basis functions", index,
                            getNumberOfBasisFunctions());
        int basePosition = 0;
        for (int j : basisFunctionSetPositions_[0])
        {
            if (j != -1)
            {
                if (index - basePosition < basisFunctionSet_->at(j)->size())
                {
                    return std::tuple<const BasisFunctionSet*, std::size_t>{basisFunctionSet_->at(j).get(), index - basePosition};
                }
                else
                {
                    basePosition += basisFunctionSet_->at(j)->size();
                }
            }
        }
        logger(ERROR, "This is not supposed to be reachable");
        return {};
    }
    
    std::tuple<const BasisFunctionSet*, std::size_t> Element::getBasisFunctionSetAndIndex(size_t index, std::size_t unknown) const
    {
        logger.assert_debug(index < getNumberOfBasisFunctions(unknown), "Asked for basis function %, but there are only % basis functions", index,
                            getNumberOfBasisFunctions(unknown));
        int basePosition = 0;
        for (int j : basisFunctionSetPositions_[unknown])
        {
            if (j != -1)
            {
                if (index - basePosition < basisFunctionSet_->at(j)->size())
                {
                    return std::tuple<const BasisFunctionSet*, std::size_t>{basisFunctionSet_->at(j).get(), index - basePosition};
                }
                else
                {
                    basePosition += basisFunctionSet_->at(j)->size();
                }
            }
        }
        logger(ERROR, "This is not supposed to be reachable");
        return {};
    }

    double Element::basisFunction(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                  std::size_t quadraturePointIndex) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(), "Asked for basis function %, but there are only % basis functions", i, getNumberOfBasisFunctions());
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i);
        return quadratureRule->eval(subSet, subIndex, quadraturePointIndex);
    }
    
    double Element::basisFunction(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                  std::size_t quadraturePointIndex, std::size_t unknown) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(unknown), "Asked for basis function %, but there are only % basis functions", i,
                            getNumberOfBasisFunctions(unknown));
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i, unknown);
        return quadratureRule->eval(subSet, subIndex, quadraturePointIndex);
    }

    double Element::basisFunctionDiv(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                     std::size_t quadraturePointIndex) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(), "Asked for basis function %, but there are only % basis functions", i, getNumberOfBasisFunctions());
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i);
        return quadratureRule->evalDiv(subSet, subIndex, quadraturePointIndex);
    }
    
    double Element::basisFunctionDiv(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                     std::size_t quadraturePointIndex, std::size_t unknown) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(unknown), "Asked for basis function %, but there are only % basis functions", i,
                            getNumberOfBasisFunctions(unknown));
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i, unknown);
        return quadratureRule->evalDiv(subSet, subIndex, quadraturePointIndex);
    }

    double Element::basisFunction(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                  std::size_t quadraturePointIndex, const Geometry::MappingReferenceToReference<1> *map) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(), "Asked for basis function %, but there are only % basis functions", i, getNumberOfBasisFunctions());
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i);
        return quadratureRule->eval(subSet, subIndex, quadraturePointIndex, map);
    }
    
    double Element::basisFunction(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                  std::size_t quadraturePointIndex, const Geometry::MappingReferenceToReference<1> *map, std::size_t unknown) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(unknown), "Asked for basis function %, but there are only % basis functions", i,
                            getNumberOfBasisFunctions(unknown));
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i, unknown);
        return quadratureRule->eval(subSet, subIndex, quadraturePointIndex, map);
    }

    double Element::basisFunctionDiv(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                     std::size_t quadraturePointIndex, const Geometry::MappingReferenceToReference<1> *map) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(), "Asked for basis function %, but there are only % basis functions", i, getNumberOfBasisFunctions());
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i);
        return quadratureRule->evalDiv(subSet, subIndex, quadraturePointIndex, map);
    }
    
    double Element::basisFunctionDiv(std::size_t i, QuadratureRules::GaussQuadratureRule *quadratureRule,
                                     std::size_t quadraturePointIndex, const Geometry::MappingReferenceToReference<1> *map, std::size_t unknown) const
    {
        logger.assert_debug(i < getNumberOfBasisFunctions(unknown), "Asked for basis function %, but there are only % basis functions", i,
                            getNumberOfBasisFunctions(unknown));
        const BasisFunctionSet *subSet;
        std::size_t subIndex;
        std::tie(subSet, subIndex) = getBasisFunctionSetAndIndex(i, unknown);
        return quadratureRule->evalDiv(subSet, subIndex, quadraturePointIndex, map);
    }

    void Element::setOwnedByCurrentProcessor(bool owned)
    {
        owned_ = owned;
    }

    bool Element::isOwnedByCurrentProcessor() const
    {
        return owned_;
    }
}

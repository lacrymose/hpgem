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
        
    Element::Element(const ElementData& otherData, const ElementGeometry& otherGeometry)
        :
        ElementGeometry(otherGeometry), ElementData(otherData)
    {        
    }
    
    ///Very ugly default constructor that's only here because it is needed in
    ///ShortTermStorageElementBase.
    Element::Element()
            : ElementData(0, 0, 0, 0, 0)
    {
    }
    
    Element::~Element()
    {        
    }
    
    Element* Element::copyWithoutFacesEdgesNodes()
    {
        //Make a new element with the data and geometry of this element
        Element* newElement = new Element(*this, *this);
        
        //copy the pointers to singletons
        newElement->quadratureRule_ = quadratureRule_;
        newElement->basisFunctionSet_ = basisFunctionSet_;
        
        //copy other data
        newElement->basisFunctionSetPositions_ = basisFunctionSetPositions_;
        newElement->id_ = id_;
        newElement->nrOfDOFinTheElement_ = nrOfDOFinTheElement_;
        newElement->orderCoeff_ = orderCoeff_;
        newElement->vecCacheData_ = vecCacheData_;
        
        //allocate memory for nodesList, facesList and edgesList
        newElement->nodesList_.resize(getNrOfNodes());
        newElement->edgesList_.resize(getNrOfEdges());
        newElement->facesList_.resize(getNrOfFaces());
        
        return newElement;
    }
    
    void Element::setDefaultBasisFunctionSet(std::size_t position)
    {
        logger.assert(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        basisFunctionSetPositions_.resize(1, -1);
        basisFunctionSetPositions_[0] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_)
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions);
        setQuadratureRulesWithOrder(orderCoeff_ * basisFunctionSet_->at(position)->getOrder() + 1);
        nrOfDOFinTheElement_ = basisFunctionSet_->at(position)->size();
    }
    
    void Element::setFaceBasisFunctionSet(std::size_t position, std::size_t localFaceIndex)
    {
        logger.assert(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        logger.assert(localFaceIndex < getNrOfFaces(), "Asked for face %, but there are only % faces", localFaceIndex, getNrOfFaces());
        if (basisFunctionSetPositions_.size() < 1 + getNrOfFaces())
        {
            basisFunctionSetPositions_.resize(1 + getNrOfFaces(), -1);
        }
        basisFunctionSetPositions_[1 + localFaceIndex] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_)
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions);
    }
    
    void Element::setEdgeBasisFunctionSet(std::size_t position, std::size_t localEdgeIndex)
    {
        logger.assert(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        logger.assert(localEdgeIndex < getNrOfEdges(), "Asked for edge %, but there are only % edges", localEdgeIndex, getNrOfEdges());
        if (basisFunctionSetPositions_.size() < 1 + getNrOfFaces() + getNrOfEdges())
        {
            basisFunctionSetPositions_.resize(1 + getNrOfFaces() + getNrOfEdges(), -1);
        }
        basisFunctionSetPositions_[1 + getNrOfFaces() + localEdgeIndex] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_)
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions);
    }
    
    void Element::setVertexBasisFunctionSet(std::size_t position, std::size_t localVertexIndex)
    {
        logger.assert(position < basisFunctionSet_->size(), "Not enough basis function sets passed");
        logger.assert(localVertexIndex < getNrOfNodes(), "Asked for node %, but there are only % nodes", localVertexIndex, getNrOfNodes());
        if (basisFunctionSetPositions_.size() < 1 + getNrOfFaces() + getNrOfEdges() + getNrOfNodes())
        {
            basisFunctionSetPositions_.resize(1 + getNrOfFaces() + getNrOfEdges() + getNrOfNodes(), -1);
        }
        basisFunctionSetPositions_[1 + getNrOfFaces() + getNrOfEdges() + localVertexIndex] = position;
        std::size_t numberOfBasisFunctions(0);
        for (int i : basisFunctionSetPositions_)
        {
            if (i != -1)
                numberOfBasisFunctions += basisFunctionSet_->at(i)->size();
        }
        setNumberOfBasisFunctions(numberOfBasisFunctions);
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
        quadratureRule_ = Geometry::ElementGeometry::referenceGeometry_->getGaussQuadratureRule(quadrROrder);
    }
    
    void Element::setGaussQuadratureRule(GaussQuadratureRuleT* const quadR)
    {
        logger.assert(quadR!=nullptr, "Invalid quadrature rule passed");
        quadratureRule_ = quadR;
    }
    
    const QuadratureRules::GaussQuadratureRule* Element::getGaussQuadratureRule() const
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
        logger.assert(i<getNrOfBasisFunctions(), "Asked for basis function %, but there are only % basis functions", i, getNrOfBasisFunctions());
        int basePosition = 0;
        for (int j : basisFunctionSetPositions_)
        {
            if (j != -1)
            {
                if (i - basePosition < basisFunctionSet_->at(j)->size())
                {
                    return basisFunctionSet_->at(j)->operator[](i - basePosition);
                }
                else
                {
                    basePosition += basisFunctionSet_->at(j)->size();
                }
            }
        }
        logger(ERROR, "This is not supposed to happen, please try again with assertions turned on");
        return nullptr;
    }
#endif
    
    void Element::setFace(std::size_t localFaceNr, const Face* face)
    {
        logger.assert(localFaceNr < getNrOfFaces(), "Asked for face %, but there are only % faces", localFaceNr, getNrOfFaces());
        logger.assert(face!=nullptr, "Invalid face passed");
        logger.assert((face->getPtrElementLeft() == this && face->localFaceNumberLeft() == localFaceNr) 
                || (face->getPtrElementRight() == this && face->localFaceNumberRight() == localFaceNr),
                      "You are only allowed to set a face to a local face index that matches");
        if (facesList_.size() < localFaceNr + 1)
        {
            logger(WARN, "Resizing the facesList, since it's smaller(%) than to localFaceNr + 1(%)", facesList_.size(), localFaceNr + 1);
            facesList_.resize(localFaceNr + 1);
        }
        facesList_[localFaceNr] = face;
    }
    
    void Element::setEdge(std::size_t localEdgeNr, const Edge* edge)
    {
        logger.assert(localEdgeNr < getNrOfEdges(), "Asked for edge %, but there are only % edges", localEdgeNr, getNrOfEdges());
        logger.assert(edge!=nullptr, "Invalid edge passed");
        //This if-statement is needed, since it could happen in 4D
        if (edgesList_.size() < localEdgeNr + 1)
        {
            edgesList_.resize(localEdgeNr + 1);
        }
        edgesList_[localEdgeNr] = edge;
    }
    
    void Element::setNode(std::size_t localNodeNr, const Node* node)
    {
        logger.assert(node!=nullptr, "Invalid node passed");
        logger.assert(std::find(nodesList_.begin(), nodesList_.end(), node) == nodesList_.end(), "Trying to add node %, but it was already added", node->getID());
        logger.assert(localNodeNr < getNrOfNodes(), "Asked for node %, but there are only % nodes", localNodeNr, getNrOfNodes());
        nodesList_[localNodeNr] = node;
    }

    std::ostream& operator<<(std::ostream& os, const Element& element)
    {
        os << '(';
        const Geometry::ElementGeometry& elemG = static_cast<const Geometry::ElementGeometry&>(element);
        operator<<(os, elemG);
        os << std::endl;
        return os;
    }

}

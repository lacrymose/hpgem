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

//----------------------------------------------------------------
#ifndef Face_h
#define Face_h
//----------------------------------------------------------------
#include "Base/FaceData.h"
#include "Base/FaceMatrix.h"
#include "Geometry/FaceGeometry.h"

namespace QuadratureRules
{
    class GaussQuadratureRule;
}

namespace Base
{
    class Element;
    struct FaceCacheData;
    
    /// Face consists of FaceGeometry and probably FaceData, if needed. 
    ///FaceGeometry holds all FaceReference related data and appropriate mappings
    //Programmer note: functions that are needed during integration should be overridden in ShortTermStorageFaceBase
    class Face : public Geometry::FaceGeometry, public FaceData
    {
    public:
        
        using ElementGeometryT = Geometry::ElementGeometry;
        using LocalFaceNrTypeT = Geometry::FaceGeometry::LocalFaceNrType;
        using CacheT = Base::FaceCacheData;
        using VecCacheT = std::vector<CacheT>;
        using FaceGeometryT = Geometry::FaceGeometry;
        using FaceQuadratureRule = QuadratureRules::GaussQuadratureRule;

    public:
        Face(Element* ptrElemL, const LocalFaceNrTypeT& localFaceNumL, 
                Element* ptrElemRight, const LocalFaceNrTypeT& localFaceNumR, 
                std::size_t faceID, std::size_t numberOfFaceMatrixes = 0, 
                std::size_t numberOfFaceVectors = 0);
        
        Face(Element* ptrElemL, const LocalFaceNrTypeT& localFaceNumL, 
        const Geometry::FaceType& ftype, std::size_t faceID, 
        std::size_t numberOfFaceMatrixes = 0, std::size_t numberOfFaceVectors = 0);
                
        ///copy constructor should not be used: if adjacent elements are the same,
        ///then the Face already exists and there is no need for another, if 
        ///adjacent elements are different, the copy is not really a copy
        Face(const Face &other) = delete;
        Face& operator=(const Face &other) = delete;
        
        ///Copy constructor with new elements. It makes a copy of the face, but 
        ///with new elements assigned to it.
        Face(const Face& other, Element* elementL, const std::size_t localFaceL, Element* elementR, const std::size_t localFaceR);

        virtual ~Face()
        {
        }
        
        /// Return the pointer to the left element.
        virtual Element* getPtrElementLeft()
        {
            return elementLeft_;
        }
        
        /// Return the pointer to the right element, nullptr if inexistent for boundaries.
        virtual Element* getPtrElementRight()
        {
            return elementRight_;
        }
        
        virtual const Element* getPtrElementLeft() const
        {
            return elementLeft_;
        }
        
        /// Return the pointer to the right element, nullptr if inexistent for boundaries.
        virtual const Element* getPtrElementRight() const
        {
            return elementRight_;
        }
        
        /// \brief Return the pointer to the element on side iSide.
        virtual const Element* getPtrElement(Side iSide) const
        {
            if (iSide == Side::LEFT)
            {
                return elementLeft_;
            }
            else
            {
                return elementRight_;
            }
        }
        
        /// \brief Create a quadrature for this face based on the quadrature rules of adjacent elements.
        void createQuadratureRules();

        void setGaussQuadratureRule(const FaceQuadratureRule* quadratureRule)
        {
            logger.assert(quadratureRule!=nullptr, "Invalid quadrature rule passed");
            quadratureRule_ = quadratureRule;
        }
        
        /// \brief Get a pointer to the quadrature rule used to do integration on this face.
        virtual const FaceQuadratureRule* getGaussQuadratureRule() const
        {
            return quadratureRule_;
        }
        
        virtual VecCacheT& getVecCacheData()
        {
            return vecCacheData_;
        }
        
        /// \brief Get the value of the basis function (corresponding to index i) at the physical point corresponding to reference point p.
        virtual double basisFunction(std::size_t i, const Geometry::PointReference& p) const;

        ///\brief returns the value of the i-th basisfunction at point p in ret
        virtual void basisFunction(std::size_t i, const Geometry::PointReference& p, LinearAlgebra::MiddleSizeVector& ret) const;

        /// \brief Returns the value of the basisfunction (corresponding to element function index iBasisFunction) on the element at side iSide at the physical point corresponding to reference point p.
        virtual double basisFunction(Side iSide, std::size_t iBasisFunction, const Geometry::PointReference& p) const;

        virtual LinearAlgebra::MiddleSizeVector basisFunctionNormal(std::size_t i, const LinearAlgebra::MiddleSizeVector& normal, const Geometry::PointReference& p) const;

        /// \brief Returns the physical normal vector multiplied by the basis function (corresponding to element function index iBasisFunction) on the element at side iSide. The value is computed at the physical point corresponding to reference point p.
        virtual LinearAlgebra::MiddleSizeVector basisFunctionNormal(Side iSide, std::size_t iBasisFunction, const LinearAlgebra::MiddleSizeVector& normal, const Geometry::PointReference& p) const;

        /// \brief Returns the (physical) derivative in direction jDir of the physical basis function (corresponding to index i) at the physical point corresponding to reference point p.
        virtual double basisFunctionDeriv(std::size_t i, std::size_t jDir, const Geometry::PointReference& p) const;

        ///\brief The "all directions in one go"-edition of basisFunctionDeriv. Also applies the scaling gained from transforming to the reference element.
        virtual LinearAlgebra::MiddleSizeVector basisFunctionDeriv(std::size_t i, const Geometry::PointReference& p) const;

        /// \brief Returns the (physical) gradient of the physical basis function (corresponding to element function index iBasisFunction) on the element at side iSide. The gradient is computed at the physical point corresponding to reference point p.
        virtual LinearAlgebra::MiddleSizeVector basisFunctionDeriv(Side iSide, std::size_t iBasisFunction, const Geometry::PointReference& p) const;

        /// \brief Returns the (physical) curl of the physical basis function (corresponding to index i) at the physical point corresponding to reference point p.
        virtual LinearAlgebra::MiddleSizeVector basisFunctionCurl(std::size_t i, const Geometry::PointReference& p) const;

        /// \brief Returns the sum of the number of basisfunctions of the adjacent elements.
        virtual std::size_t getNrOfBasisFunctions() const;

        virtual std::size_t getLocalNrOfBasisFunctions() const
        {
            return nrOfConformingDOFOnTheFace_;
        }
        
        void setLocalNrOfBasisFunctions(std::size_t number)
        {
            nrOfConformingDOFOnTheFace_ = number;
        }
        
        virtual std::size_t getID() const
        {
            return faceID_;
        }
        
        /// Specify a time level index, return a vector containing the data for that time level.
        virtual LinearAlgebra::MiddleSizeVector getTimeLevelData(std::size_t timeLevel, std::size_t unknown = 0) const;

        /// \brief Convert the side of the face, the index corresponding to the scalar basis function (scalarBasisFunctionId) and the index corresponding to the variable (varId) to a single index (faceBasisFunctionId).
        virtual std::size_t convertToSingleIndex(Side side, std::size_t scalarBasisFunctionId, std::size_t varId = 0) const;
        
        /// \brief Convert the index of the basis (vector)function of the face (faceBasisFunctionId) to the corresponding side of the face.
        virtual Side getSide(std::size_t faceBasisFunctionId) const;
        
        /// \brief Convert the index of the basis (vector)function of the face (faceBasisFunctionId) to the index of the corresponding element basis (vector)function (elementBasisFunctionId).
        virtual std::size_t getElementBasisFunctionId(std::size_t faceBasisFunctionId) const;

    protected:
        
        ///\brief default constructor - for use with wrapper classes
        Face() : FaceGeometry(), FaceData(0, 0, 0), elementLeft_(nullptr), elementRight_(nullptr), quadratureRule_(nullptr)
        {
        }
        
    private:
        

        Element* elementLeft_;
        Element* elementRight_;
        const FaceQuadratureRule* quadratureRule_;

        std::size_t nrOfConformingDOFOnTheFace_;
        std::size_t faceID_;
    };
}
#endif

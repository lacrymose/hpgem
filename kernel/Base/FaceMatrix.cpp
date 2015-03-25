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
#include "FaceMatrix.h"

#include "Logger.h"
#include <iostream>

namespace Base
{
    class FaceMatrix;
    
    // Constructors
    /// \param[in] nDOFLeft The number of degrees of freedom corresponding to the left element.
    /// \param[in] nDOFRight The number of degrees of freedom corresponding to the right element.
    FaceMatrix::FaceMatrix(const std::size_t nDOFLeft, const std::size_t nDOFRight)
            : M_LeftLeft_(nDOFLeft, nDOFLeft), M_LeftRight_(nDOFLeft, nDOFRight), M_RightLeft_(nDOFRight, nDOFLeft), M_RightRight_(nDOFRight, nDOFRight)
    {
    }
    
    /// \param[in] other FaceMatrix that will be copied to construct a new FaceMatrix.
    FaceMatrix::FaceMatrix(const FaceMatrix &other)
            : M_LeftLeft_(other.M_LeftLeft_), M_LeftRight_(other.M_LeftRight_), M_RightLeft_(other.M_RightLeft_), M_RightRight_(other.M_RightRight_)
    {
    }
    
    // Operators
    /// \param[in] iSide Side of the adjacent element to consider the test function.
    /// \param[in] jSide Side of the adjacent element to consider the solution.
    /// \param[in] iVarBasisFunction Index for both the variable and basis function corresponding to a test function at the element on side iSide.
    /// \param[in] jVarBasisFunction Index for both the variable and basis function corresponding to the solution at the element on side jSide.
    double & FaceMatrix::operator()(Side iSide, Side jSide, std::size_t iVarBasisFunction, std::size_t jVarBasisFunction)
    {
        logger.assert(iVarBasisFunction < getNrOfDegreesOfFreedom(iSide), "Asked for degree of freedom %, but there are only % degrees of freedom", iVarBasisFunction, getNrOfDegreesOfFreedom(iSide));
        logger.assert(jVarBasisFunction < getNrOfDegreesOfFreedom(jSide), "Asked for degree of freedom %, but there are only % degrees of freedom", jVarBasisFunction, getNrOfDegreesOfFreedom(jSide));
        if (iSide == Side::LEFT)
        {
            if (jSide == Side::LEFT)
            {
                return M_LeftLeft_(iVarBasisFunction, jVarBasisFunction);
            }
            else
            {
                return M_LeftRight_(iVarBasisFunction, jVarBasisFunction);
            }
        }
        else
        {
            if (jSide == Side::LEFT)
            {
                return M_RightLeft_(iVarBasisFunction, jVarBasisFunction);
            }
            else
            {
                return M_RightRight_(iVarBasisFunction, jVarBasisFunction);
            }
        }
    }
    
    /// \param[in] i Index for the basis (vector-)function corresponding to the test function.
    /// \param[in] j Index for the basis (vector-)function corresponding to the solution.
    ///
    /// \details
    /// The index i indicates the side of the adjacent element as well as the variable and basis function at this element corresponding to the test function. Idem for j, but now for the solution instead of the test function.
    double & FaceMatrix::operator()(std::size_t i, std::size_t j)
    {
        logger.assert(i < getNrOfDegreesOfFreedom(Side::LEFT) + getNrOfDegreesOfFreedom(Side::RIGHT), "Asked for degree of freedom %, but there are only % degrees of freedom", i, getNrOfDegreesOfFreedom(Side::LEFT) + getNrOfDegreesOfFreedom(Side::RIGHT));
        logger.assert(j < getNrOfDegreesOfFreedom(Side::LEFT) + getNrOfDegreesOfFreedom(Side::RIGHT), "Asked for degree of freedom %, but there are only % degrees of freedom", j, getNrOfDegreesOfFreedom(Side::LEFT) + getNrOfDegreesOfFreedom(Side::RIGHT));
        std::size_t nDOFLeft = M_LeftLeft_.getNRows();
        if (i < nDOFLeft)
        {
            if (j < nDOFLeft)
            {
                return M_LeftLeft_(i, j);
            }
            else
            {
                return M_LeftRight_(i, j - nDOFLeft);
            }
        }
        else
        {
            if (j < nDOFLeft)
            {
                return M_RightLeft_(i - nDOFLeft, j);
            }
            else
            {
                return M_RightRight_(i - nDOFLeft, j - nDOFLeft);
            }
        }
    }
    
    /// \param[in] other FaceMatrix that is being copied.
    FaceMatrix & FaceMatrix::operator=(const FaceMatrix &other)
    {
        M_LeftLeft_ = other.M_LeftLeft_;
        M_LeftRight_ = other.M_LeftRight_;
        M_RightLeft_ = other.M_RightLeft_;
        M_RightRight_ = other.M_RightRight_;
        return *this;
    }
    
    /// \param[in] other FaceMatrix that is being added.
    FaceMatrix & FaceMatrix::operator+=(const FaceMatrix &other)
    {
        M_LeftLeft_ += other.M_LeftLeft_;
        M_LeftRight_ += other.M_LeftRight_;
        M_RightLeft_ += other.M_RightLeft_;
        M_RightRight_ += other.M_RightRight_;
        return *this;
    }
    
    /// \param[in] scalar Factor with which the FaceMatrix is multiplied.
    FaceMatrix & FaceMatrix::operator*=(const double &scalar)
    {
        M_LeftLeft_ *= scalar;
        M_LeftRight_ *= scalar;
        M_RightLeft_ *= scalar;
        M_RightRight_ *= scalar;
        return *this;
    }
    
    // Other member functions
    /// \param[in] nDOFLeft The number of degrees of freedom corresponding to the left element.
    /// \param[in] nDOFRight The number of degrees of freedom corresponding to the right element.
    void FaceMatrix::resize(const std::size_t nDOFLeft, const std::size_t nDOFRight)
    {
        M_LeftLeft_.resize(nDOFLeft, nDOFLeft);
        M_LeftRight_.resize(nDOFLeft, nDOFRight);
        M_RightLeft_.resize(nDOFRight, nDOFLeft);
        M_RightRight_.resize(nDOFRight, nDOFRight);
    }
    
    /// \param[in] iSide Side of the adjacent element to consider the test function.
    /// \param[in] jSide Side of the adjacent element to consider the solution.
    const LinearAlgebra::Matrix & FaceMatrix::getElementMatrix(Side iSide, Side jSide) const
    {
        if (iSide == Side::LEFT)
        {
            if (jSide == Side::LEFT)
            {
                return M_LeftLeft_;
            }
            else
            {
                return M_LeftRight_;
            }
        }
        else
        {
            if (jSide == Side::LEFT)
            {
                return M_RightLeft_;
            }
            else
            {
                return M_RightRight_;
            }
        }
    }
    
    /// \param[in] elementMatrix The matrix used to set the element matrix corresponding to sides iSide and jSide.
    /// \param[in] iSide Side of the adjacent element to consider the test function.
    /// \param[in] jSide Side of the adjacent element to consider the solution.
    void FaceMatrix::setElementMatrix(const LinearAlgebra::Matrix & elementMatrix, Side iSide, Side jSide)
    {
        // Check size of the elementMatrix.
        logger.assert(elementMatrix.getNRows() == getNrOfDegreesOfFreedom(iSide), "elementMatrix has the wrong size.");
        logger.assert(elementMatrix.getNCols() == getNrOfDegreesOfFreedom(jSide), "elementMatrix has the wrong size.");
        
        if (iSide == Side::LEFT)
        {
            if (jSide == Side::LEFT)
            {
                M_LeftLeft_ = elementMatrix;
            }
            else
            {
                M_LeftRight_ = elementMatrix;
            }
        }
        else
        {
            if (jSide == Side::LEFT)
            {
                M_RightLeft_ = elementMatrix;
            }
            else
            {
                M_RightRight_ = elementMatrix;
            }
        }
    }
    
    /// \details This function will be slow compared to getElementMatrix. It is 
    /// advised to use getElementMatrix instead when possible.
    const LinearAlgebra::Matrix FaceMatrix::getEntireMatrix() const
    {
        std::size_t nDOFLeft = M_LeftLeft_.getNRows();
        std::size_t nDOFRight = M_RightRight_.getNRows();
        LinearAlgebra::Matrix entireMatrix(nDOFLeft + nDOFRight, nDOFLeft + nDOFRight);
        
        // This is probably slow and inefficient.
        for (std::size_t i = 0; i < nDOFLeft; i++)
        {
            for (std::size_t j = 0; j < nDOFLeft; j++)
            {
                entireMatrix(i, j) = M_LeftLeft_(i, j);
            }
        }
        for (std::size_t i = 0; i < nDOFLeft; i++)
        {
            for (std::size_t j = 0; j < nDOFRight; j++)
            {
                entireMatrix(i, nDOFLeft + j) = M_LeftRight_(i, j);
            }
        }
        for (std::size_t i = 0; i < nDOFRight; i++)
        {
            for (std::size_t j = 0; j < nDOFLeft; j++)
            {
                entireMatrix(nDOFLeft + i, j) = M_RightLeft_(i, j);
            }
        }
        for (std::size_t i = 0; i < nDOFRight; i++)
        {
            for (std::size_t j = 0; j < nDOFRight; j++)
            {
                entireMatrix(nDOFLeft + i, nDOFLeft + j) = M_RightRight_(i, j);
            }
        }
        
        return entireMatrix;
    }
    
    /// \param[in] entireMatrix The standard matrix used to set the face matrix.
    /// \details This function will be slow compared to setElementMatrix. It is advised to use setElementMatrix instead when possible.
    void FaceMatrix::setEntireMatrix(const LinearAlgebra::Matrix & entireMatrix)
    {
        std::size_t nDOFLeft = M_LeftLeft_.getNRows();
        std::size_t nDOFRight = M_RightRight_.getNRows();
        
        // Check size of entireMatrix.
        logger.assert(entireMatrix.getNRows() == nDOFLeft + nDOFRight, "elementMatrix has the wrong size.");
        logger.assert(entireMatrix.getNCols() == nDOFLeft + nDOFRight, "elementMatrix has the wrong size.");
        
        // This is probably slow and inefficient.
        for (std::size_t i = 0; i < nDOFLeft; i++)
        {
            for (std::size_t j = 0; j < nDOFLeft; j++)
            {
                M_LeftLeft_(i, j) = entireMatrix(i, j);
            }
        }
        for (std::size_t i = 0; i < nDOFLeft; i++)
        {
            for (std::size_t j = 0; j < nDOFRight; j++)
            {
                M_LeftRight_(i, j) = entireMatrix(i, nDOFLeft + j);
            }
        }
        for (std::size_t i = 0; i < nDOFRight; i++)
        {
            for (std::size_t j = 0; j < nDOFLeft; j++)
            {
                M_RightLeft_(i, j) = entireMatrix(nDOFLeft + i, j);
            }
        }
        for (std::size_t i = 0; i < nDOFRight; i++)
        {
            for (std::size_t j = 0; j < nDOFRight; j++)
            {
                M_RightRight_(i, j) = entireMatrix(nDOFLeft + i, nDOFLeft + j);
            }
        }
    }
    
    /// \param[in] a Factor with which FaceMatrix x is multiplied.
    /// \param[in] x FaceMatrix which will be scaled by a factor a and then added.
    void FaceMatrix::axpy(const double &a, const FaceMatrix &x)
    {
        M_LeftLeft_.axpy(a, x.M_LeftLeft_);
        M_LeftRight_.axpy(a, x.M_LeftRight_);
        M_RightLeft_.axpy(a, x.M_RightLeft_);
        M_RightRight_.axpy(a, x.M_RightRight_);
    }
}
;
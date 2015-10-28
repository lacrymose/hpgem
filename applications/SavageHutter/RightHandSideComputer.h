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

#ifndef RIGHTHANDSIDECOMPUTER_H
#define	RIGHTHANDSIDECOMPUTER_H
#include "LinearAlgebra/MiddleSizeVector.h"
#include "Base/PhysicalElement.h"
#include "GlobalConstants.h"

using LinearAlgebra::MiddleSizeVector;

class RightHandSideComputer
{
public:

    RightHandSideComputer(std::size_t numVars, double chuteAngle)
    : numOfVariables_(numVars), chuteAngle_(chuteAngle) { }
    
    virtual ~RightHandSideComputer(){ }

    /// \brief Purely virtual function to compute the integrand for the right hand side for the reference element.
    virtual MiddleSizeVector integrandRightHandSideOnElement
    (
        Base::PhysicalElement<DIM> &element,
        const double &time,
        const MiddleSizeVector &solutionCoefficients
        ) = 0;

    /// \brief Purely virtual function to compute the integrand for the right hand side for the reference face corresponding to a boundary face.
    virtual MiddleSizeVector integrandRightHandSideOnRefFace
    (
        Base::PhysicalFace<DIM> &face,
        const MiddleSizeVector &solutionCoefficients,
        const double time
        ) = 0;

    /// \brief Purely virtual function to compute the integrand for the right hand side for the reference face corresponding to an internal face.
    virtual MiddleSizeVector integrandRightHandSideOnRefFace
    (
        Base::PhysicalFace<DIM> &face,
        const Base::Side &iSide,
        const MiddleSizeVector &solutionCoefficientsLeft,
        const MiddleSizeVector &solutionCoefficientsRight
        ) = 0;
    
    virtual void setInflowBC(MiddleSizeVector inflowBC){ }
    
     ///\brief set the angle of the chute, in radians
    void setChuteAngle(const double angle)
    {
        chuteAngle_ = angle;
    }
    
    ///\brief chute angle is in radians
    double getChuteAngle()
    {
        return chuteAngle_;
    }

protected:
    std::size_t numOfVariables_;
    double chuteAngle_;
};

#endif	/* RIGHTHANDSIDECOMPUTER_H */


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

//------------------------------------------------------------------------------
// File: L2Norm.cc
//------------------------------------------------------------------------------
// System includes and names imported from them:
#include <cmath>
//------------------------------------------------------------------------------
#include "L2Norm.h"
#include "LinearAlgebra/NumericalVector.h"
#include "Geometry/PointPhysical.h"
//------------------------------------------------------------------------------
namespace Base
{
    /*template <>
     double L2Norm<1>(const LinearAlgebra::NumericalVector& v)
     {
     return std::abs(v[0]);
     }
     
     template <>
     double L2Norm<2>(const LinearAlgebra::NumericalVector& v)
     {
     return std::sqrt(v[0] * v[0] + v[1] * v[1]);
     }

     template <>
     double L2Norm<3>(const LinearAlgebra::NumericalVector& v)
     {
     return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
     }*/

    double L2Norm(const LinearAlgebra::NumericalVector& v)
    {
        double retSquared(0);
        for (std::size_t i = 0; i < v.size(); ++i)
        {
            retSquared += v[i] * v[i];
        }
        return std::sqrt(retSquared);
    }
    
    /*template <>
     double L2Norm<1>(const Geometry::PointPhysical<1>& v)
     {
     return std::abs(v[0]);
     }
     
     template <>
     double L2Norm<2>(const Geometry::PointPhysical<2>& v)
     {
     return std::sqrt(v[0] * v[0] + v[1] * v[1]);
     }

     template <>
     double L2Norm<3>(const Geometry::PointPhysical<3>& v)
     {
     return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
     }*/

    ///\deprecated
    double L2Norm(const Geometry::PointPhysical& v)
    {
        double retSquared(0);
        for (std::size_t i = 0; i < v.size(); ++i)
        {
            retSquared += v[i] * v[i];
        }
        return std::sqrt(retSquared);
    }
}
;


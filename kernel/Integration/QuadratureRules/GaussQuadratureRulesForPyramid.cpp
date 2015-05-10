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
//---------------------------------------------------------------------------
// System includes and names imported from them:
#include <cmath>
//---------------------------------------------------------------------------
// Package includes:
#include "Integration/GlobalNamespaceIntegration.h"
#include "Integration/QuadratureRules/GaussQuadratureRulesForPyramid.h"
#include "Geometry/ReferencePyramid.h"
#include "Geometry/PointReference.h"
using Geometry::ReferencePyramid;

//---------------------------------------------------------------------------
namespace QuadratureRules
{
//---------------------------------------------------------------------------
    std::string Pyramid_1_1::getName() const
    {
        return name_;
    }
    
    std::size_t Pyramid_1_1::order() const
    {
        return 1;
    }
    
    std::size_t Pyramid_1_1::dimension() const
    {
        return 3;
    }
    
    std::size_t Pyramid_1_1::nrOfPoints() const
    {
        return 4;
    }
    
    double Pyramid_1_1::weight(std::size_t i) const
    {
        if (i < 4)
            return weight_[i];
        else
            throw name_ + "::weight - wrong index!";
    }
    
    const Geometry::PointReference&
    Pyramid_1_1::getPoint(std::size_t i) const
    {
        if (i < nrOfPoints())
            return gp_[i];
        else
            throw name_ + "::getPoint -  wrong index!";
    }
    
    Pyramid_1_1::ReferenceGeometryT*
    Pyramid_1_1::forReferenceGeometry() const
    {
        return refGeoPtr_;
    }
    
    Pyramid_1_1::Pyramid_1_1()
            : name_("Pyramid_1_1"), refGeoPtr_(&ReferencePyramid::Instance()), gp_(4, 3)
    {
        weight_[0] = ((2.0) * (2.0)) * (0.1108884156);
        gp_[0][0] = (0.0) * (1. - (0.4850054945e-1));
        gp_[0][1] = (0.0) * (1. - (0.4850054945e-1));
        gp_[0][2] = 0.4850054945e-1;
        
        weight_[1] = ((2.0) * (2.0)) * (0.1434587898);
        gp_[1][0] = (0.0) * (1. - (0.2386007376));
        gp_[1][1] = (0.0) * (1. - (0.2386007376));
        gp_[1][2] = 0.2386007376;
        
        weight_[2] = ((2.0) * (2.0)) * (0.6863388717e-1);
        gp_[2][0] = (0.0) * (1. - (0.5170472951));
        gp_[2][1] = (0.0) * (1. - (0.5170472951));
        gp_[2][2] = 0.5170472951;
        
        weight_[3] = ((2.0) * (2.0)) * (0.1035224075e-1);
        gp_[3][0] = (0.0) * (1. - (0.7958514179));
        gp_[3][1] = (0.0) * (1. - (0.7958514179));
        gp_[3][2] = 0.7958514179;
        
    }
    
    Pyramid_1_1::~Pyramid_1_1()
    {
    }
    
//---------------------------------------------------------------------------
    std::string Pyramid_3_1::getName() const
    {
        return name_;
    }
    
    std::size_t Pyramid_3_1::order() const
    {
        return 3;
    }
    
    std::size_t Pyramid_3_1::dimension() const
    {
        return 3;
    }
    
    std::size_t Pyramid_3_1::nrOfPoints() const
    {
        return 16;
    }
    
    double Pyramid_3_1::weight(std::size_t i) const
    {
        if (i < 16)
            return weight_[i];
        else
            throw name_ + "::weight - wrong index!";
    }
    
    const Geometry::PointReference&
    Pyramid_3_1::getPoint(std::size_t i) const
    {
        if (i < nrOfPoints())
            return gp_[i];
        else
            throw name_ + "::getPoint -  wrong index!";
    }
    
    Pyramid_3_1::ReferenceGeometryT*
    Pyramid_3_1::forReferenceGeometry() const
    {
        return refGeoPtr_;
    }
    
    Pyramid_3_1::Pyramid_3_1()
            : name_("Pyramid_3_1"), refGeoPtr_(&ReferencePyramid::Instance()), gp_(16, 3)
    {
        weight_[0] = ((1.0) * (1.0)) * (0.1108884156);
        gp_[0][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[0][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[0][2] = 0.4850054945e-1;
        
        weight_[1] = ((1.0) * (1.0)) * (0.1108884156);
        gp_[1][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[1][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[1][2] = 0.4850054945e-1;
        
        weight_[2] = ((1.0) * (1.0)) * (0.1108884156);
        gp_[2][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[2][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[2][2] = 0.4850054945e-1;
        
        weight_[3] = ((1.0) * (1.0)) * (0.1108884156);
        gp_[3][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[3][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.4850054945e-1));
        gp_[3][2] = 0.4850054945e-1;
        
        weight_[4] = ((1.0) * (1.0)) * (0.1434587898);
        gp_[4][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[4][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[4][2] = 0.2386007376;
        
        weight_[5] = ((1.0) * (1.0)) * (0.1434587898);
        gp_[5][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[5][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[5][2] = 0.2386007376;
        
        weight_[6] = ((1.0) * (1.0)) * (0.1434587898);
        gp_[6][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[6][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[6][2] = 0.2386007376;
        
        weight_[7] = ((1.0) * (1.0)) * (0.1434587898);
        gp_[7][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[7][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.2386007376));
        gp_[7][2] = 0.2386007376;
        
        weight_[8] = ((1.0) * (1.0)) * (0.6863388717e-1);
        gp_[8][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[8][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[8][2] = 0.5170472951;
        
        weight_[9] = ((1.0) * (1.0)) * (0.6863388717e-1);
        gp_[9][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[9][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[9][2] = 0.5170472951;
        
        weight_[10] = ((1.0) * (1.0)) * (0.6863388717e-1);
        gp_[10][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[10][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[10][2] = 0.5170472951;
        
        weight_[11] = ((1.0) * (1.0)) * (0.6863388717e-1);
        gp_[11][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[11][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.5170472951));
        gp_[11][2] = 0.5170472951;
        
        weight_[12] = ((1.0) * (1.0)) * (0.1035224075e-1);
        gp_[12][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[12][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[12][2] = 0.7958514179;
        
        weight_[13] = ((1.0) * (1.0)) * (0.1035224075e-1);
        gp_[13][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[13][1] = (-std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[13][2] = 0.7958514179;
        
        weight_[14] = ((1.0) * (1.0)) * (0.1035224075e-1);
        gp_[14][0] = (-std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[14][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[14][2] = 0.7958514179;
        
        weight_[15] = ((1.0) * (1.0)) * (0.1035224075e-1);
        gp_[15][0] = (+std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[15][1] = (+std::sqrt(3.0) / 3.0) * (1. - (0.7958514179));
        gp_[15][2] = 0.7958514179;
        
    }
    
    Pyramid_3_1::~Pyramid_3_1()
    {
    }
    
//---------------------------------------------------------------------------
    std::string Pyramid_5_1::getName() const
    {
        return name_;
    }
    
    std::size_t Pyramid_5_1::order() const
    {
        return 5;
    }
    
    std::size_t Pyramid_5_1::dimension() const
    {
        return 3;
    }
    
    std::size_t Pyramid_5_1::nrOfPoints() const
    {
        return 36;
    }
    
    double Pyramid_5_1::weight(std::size_t i) const
    {
        if (i < 36)
            return weight_[i];
        else
            throw name_ + "::weight - wrong index!";
    }
    
    const Geometry::PointReference&
    Pyramid_5_1::getPoint(std::size_t i) const
    {
        if (i < nrOfPoints())
            return gp_[i];
        else
            throw name_ + "::getPoint -  wrong index!";
    }
    
    Pyramid_5_1::ReferenceGeometryT*
    Pyramid_5_1::forReferenceGeometry() const
    {
        return refGeoPtr_;
    }
    
    Pyramid_5_1::Pyramid_5_1()
            : name_("Pyramid_5_1"), refGeoPtr_(&ReferencePyramid::Instance()), gp_(36, 3)
    {
        weight_[0] = ((5. / 9.) * (5. / 9.)) * (0.1108884156);
        gp_[0][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[0][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[0][2] = 0.4850054945e-1;
        
        weight_[1] = ((8. / 9.) * (5. / 9.)) * (0.1108884156);
        gp_[1][0] = (0.0) * (1. - (0.4850054945e-1));
        gp_[1][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[1][2] = 0.4850054945e-1;
        
        weight_[2] = ((5. / 9.) * (5. / 9.)) * (0.1108884156);
        gp_[2][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[2][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[2][2] = 0.4850054945e-1;
        
        weight_[3] = ((5. / 9.) * (8. / 9.)) * (0.1108884156);
        gp_[3][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[3][1] = (0.0) * (1. - (0.4850054945e-1));
        gp_[3][2] = 0.4850054945e-1;
        
        weight_[4] = ((8. / 9.) * (8. / 9.)) * (0.1108884156);
        gp_[4][0] = (0.0) * (1. - (0.4850054945e-1));
        gp_[4][1] = (0.0) * (1. - (0.4850054945e-1));
        gp_[4][2] = 0.4850054945e-1;
        
        weight_[5] = ((5. / 9.) * (8. / 9.)) * (0.1108884156);
        gp_[5][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[5][1] = (0.0) * (1. - (0.4850054945e-1));
        gp_[5][2] = 0.4850054945e-1;
        
        weight_[6] = ((5. / 9.) * (5. / 9.)) * (0.1108884156);
        gp_[6][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[6][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[6][2] = 0.4850054945e-1;
        
        weight_[7] = ((8. / 9.) * (5. / 9.)) * (0.1108884156);
        gp_[7][0] = (0.0) * (1. - (0.4850054945e-1));
        gp_[7][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[7][2] = 0.4850054945e-1;
        
        weight_[8] = ((5. / 9.) * (5. / 9.)) * (0.1108884156);
        gp_[8][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[8][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.4850054945e-1));
        gp_[8][2] = 0.4850054945e-1;
        
        weight_[9] = ((5. / 9.) * (5. / 9.)) * (0.1434587898);
        gp_[9][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[9][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[9][2] = 0.2386007376;
        
        weight_[10] = ((8. / 9.) * (5. / 9.)) * (0.1434587898);
        gp_[10][0] = (0.0) * (1. - (0.2386007376));
        gp_[10][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[10][2] = 0.2386007376;
        
        weight_[11] = ((5. / 9.) * (5. / 9.)) * (0.1434587898);
        gp_[11][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[11][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[11][2] = 0.2386007376;
        
        weight_[12] = ((5. / 9.) * (8. / 9.)) * (0.1434587898);
        gp_[12][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[12][1] = (0.0) * (1. - (0.2386007376));
        gp_[12][2] = 0.2386007376;
        
        weight_[13] = ((8. / 9.) * (8. / 9.)) * (0.1434587898);
        gp_[13][0] = (0.0) * (1. - (0.2386007376));
        gp_[13][1] = (0.0) * (1. - (0.2386007376));
        gp_[13][2] = 0.2386007376;
        
        weight_[14] = ((5. / 9.) * (8. / 9.)) * (0.1434587898);
        gp_[14][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[14][1] = (0.0) * (1. - (0.2386007376));
        gp_[14][2] = 0.2386007376;
        
        weight_[15] = ((5. / 9.) * (5. / 9.)) * (0.1434587898);
        gp_[15][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[15][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[15][2] = 0.2386007376;
        
        weight_[16] = ((8. / 9.) * (5. / 9.)) * (0.1434587898);
        gp_[16][0] = (0.0) * (1. - (0.2386007376));
        gp_[16][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[16][2] = 0.2386007376;
        
        weight_[17] = ((5. / 9.) * (5. / 9.)) * (0.1434587898);
        gp_[17][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[17][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.2386007376));
        gp_[17][2] = 0.2386007376;
        
        weight_[18] = ((5. / 9.) * (5. / 9.)) * (0.6863388717e-1);
        gp_[18][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[18][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[18][2] = 0.5170472951;
        
        weight_[19] = ((8. / 9.) * (5. / 9.)) * (0.6863388717e-1);
        gp_[19][0] = (0.0) * (1. - (0.5170472951));
        gp_[19][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[19][2] = 0.5170472951;
        
        weight_[20] = ((5. / 9.) * (5. / 9.)) * (0.6863388717e-1);
        gp_[20][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[20][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[20][2] = 0.5170472951;
        
        weight_[21] = ((5. / 9.) * (8. / 9.)) * (0.6863388717e-1);
        gp_[21][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[21][1] = (0.0) * (1. - (0.5170472951));
        gp_[21][2] = 0.5170472951;
        
        weight_[22] = ((8. / 9.) * (8. / 9.)) * (0.6863388717e-1);
        gp_[22][0] = (0.0) * (1. - (0.5170472951));
        gp_[22][1] = (0.0) * (1. - (0.5170472951));
        gp_[22][2] = 0.5170472951;
        
        weight_[23] = ((5. / 9.) * (8. / 9.)) * (0.6863388717e-1);
        gp_[23][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[23][1] = (0.0) * (1. - (0.5170472951));
        gp_[23][2] = 0.5170472951;
        
        weight_[24] = ((5. / 9.) * (5. / 9.)) * (0.6863388717e-1);
        gp_[24][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[24][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[24][2] = 0.5170472951;
        
        weight_[25] = ((8. / 9.) * (5. / 9.)) * (0.6863388717e-1);
        gp_[25][0] = (0.0) * (1. - (0.5170472951));
        gp_[25][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[25][2] = 0.5170472951;
        
        weight_[26] = ((5. / 9.) * (5. / 9.)) * (0.6863388717e-1);
        gp_[26][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[26][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.5170472951));
        gp_[26][2] = 0.5170472951;
        
        weight_[27] = ((5. / 9.) * (5. / 9.)) * (0.1035224075e-1);
        gp_[27][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[27][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[27][2] = 0.7958514179;
        
        weight_[28] = ((8. / 9.) * (5. / 9.)) * (0.1035224075e-1);
        gp_[28][0] = (0.0) * (1. - (0.7958514179));
        gp_[28][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[28][2] = 0.7958514179;
        
        weight_[29] = ((5. / 9.) * (5. / 9.)) * (0.1035224075e-1);
        gp_[29][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[29][1] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[29][2] = 0.7958514179;
        
        weight_[30] = ((5. / 9.) * (8. / 9.)) * (0.1035224075e-1);
        gp_[30][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[30][1] = (0.0) * (1. - (0.7958514179));
        gp_[30][2] = 0.7958514179;
        
        weight_[31] = ((8. / 9.) * (8. / 9.)) * (0.1035224075e-1);
        gp_[31][0] = (0.0) * (1. - (0.7958514179));
        gp_[31][1] = (0.0) * (1. - (0.7958514179));
        gp_[31][2] = 0.7958514179;
        
        weight_[32] = ((5. / 9.) * (8. / 9.)) * (0.1035224075e-1);
        gp_[32][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[32][1] = (0.0) * (1. - (0.7958514179));
        gp_[32][2] = 0.7958514179;
        
        weight_[33] = ((5. / 9.) * (5. / 9.)) * (0.1035224075e-1);
        gp_[33][0] = (-std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[33][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[33][2] = 0.7958514179;
        
        weight_[34] = ((8. / 9.) * (5. / 9.)) * (0.1035224075e-1);
        gp_[34][0] = (0.0) * (1. - (0.7958514179));
        gp_[34][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[34][2] = 0.7958514179;
        
        weight_[35] = ((5. / 9.) * (5. / 9.)) * (0.1035224075e-1);
        gp_[35][0] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[35][1] = (+std::sqrt(3.0 / 5.0)) * (1. - (0.7958514179));
        gp_[35][2] = 0.7958514179;
        
    }
    
    Pyramid_5_1::~Pyramid_5_1()
    {
    }
    
//---------------------------------------------------------------------------
    std::string Pyramid_7_1::getName() const
    {
        return name_;
    }
    
    std::size_t Pyramid_7_1::order() const
    {
        return 7;
    }
    
    std::size_t Pyramid_7_1::dimension() const
    {
        return 3;
    }
    
    std::size_t Pyramid_7_1::nrOfPoints() const
    {
        return 48;
    }
    
    double Pyramid_7_1::weight(std::size_t i) const
    {
        if (i < 48)
            return weight_[i];
        else
            throw name_ + "::weight - wrong index!";
    }
    
    const Geometry::PointReference&
    Pyramid_7_1::getPoint(std::size_t i) const
    {
        if (i < nrOfPoints())
            return gp_[i];
        else
            throw name_ + "::getPoint -  wrong index!";
    }
    
    Pyramid_7_1::ReferenceGeometryT*
    Pyramid_7_1::forReferenceGeometry() const
    {
        return refGeoPtr_;
    }
    
    Pyramid_7_1::Pyramid_7_1()
            : name_("Pyramid_7_1"), refGeoPtr_(&ReferencePyramid::Instance()), gp_(48, 3)
    {
        weight_[0] = (98. / 405.) * (0.1108884156);
        gp_[0][0] = (+(std::sqrt(6. / 7.))) * (1. - (0.4850054945e-1));
        gp_[0][1] = (0.) * (1. - (0.4850054945e-1));
        gp_[0][2] = 0.4850054945e-1;
        
        weight_[1] = (98. / 405.) * (0.1108884156);
        gp_[1][0] = (-(std::sqrt(6. / 7.))) * (1. - (0.4850054945e-1));
        gp_[1][1] = (0.) * (1. - (0.4850054945e-1));
        gp_[1][2] = 0.4850054945e-1;
        
        weight_[2] = (98. / 405.) * (0.1108884156);
        gp_[2][0] = (0.) * (1. - (0.4850054945e-1));
        gp_[2][1] = (+(std::sqrt(6. / 7.))) * (1. - (0.4850054945e-1));
        gp_[2][2] = 0.4850054945e-1;
        
        weight_[3] = (98. / 405.) * (0.1108884156);
        gp_[3][0] = (0.) * (1. - (0.4850054945e-1));
        gp_[3][1] = (-(std::sqrt(6. / 7.))) * (1. - (0.4850054945e-1));
        gp_[3][2] = 0.4850054945e-1;
        
        weight_[4] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[4][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[4][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[4][2] = 0.4850054945e-1;
        
        weight_[5] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[5][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[5][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[5][2] = 0.4850054945e-1;
        
        weight_[6] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[6][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[6][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[6][2] = 0.4850054945e-1;
        
        weight_[7] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[7][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[7][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[7][2] = 0.4850054945e-1;
        
        weight_[8] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[8][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[8][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[8][2] = 0.4850054945e-1;
        
        weight_[9] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[9][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[9][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[9][2] = 0.4850054945e-1;
        
        weight_[10] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[10][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[10][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[10][2] = 0.4850054945e-1;
        
        weight_[11] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1108884156);
        gp_[11][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[11][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.4850054945e-1));
        gp_[11][2] = 0.4850054945e-1;
        
        weight_[12] = (98. / 405.) * (0.1434587898);
        gp_[12][0] = (+(std::sqrt(6. / 7.))) * (1. - (0.2386007376));
        gp_[12][1] = (0.) * (1. - (0.2386007376));
        gp_[12][2] = 0.2386007376;
        
        weight_[13] = (98. / 405.) * (0.1434587898);
        gp_[13][0] = (-(std::sqrt(6. / 7.))) * (1. - (0.2386007376));
        gp_[13][1] = (0.) * (1. - (0.2386007376));
        gp_[13][2] = 0.2386007376;
        
        weight_[14] = (98. / 405.) * (0.1434587898);
        gp_[14][0] = (0.) * (1. - (0.2386007376));
        gp_[14][1] = (+(std::sqrt(6. / 7.))) * (1. - (0.2386007376));
        gp_[14][2] = 0.2386007376;
        
        weight_[15] = (98. / 405.) * (0.1434587898);
        gp_[15][0] = (0.) * (1. - (0.2386007376));
        gp_[15][1] = (-(std::sqrt(6. / 7.))) * (1. - (0.2386007376));
        gp_[15][2] = 0.2386007376;
        
        weight_[16] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[16][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[16][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[16][2] = 0.2386007376;
        
        weight_[17] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[17][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[17][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[17][2] = 0.2386007376;
        
        weight_[18] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[18][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[18][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[18][2] = 0.2386007376;
        
        weight_[19] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[19][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[19][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[19][2] = 0.2386007376;
        
        weight_[20] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[20][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[20][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[20][2] = 0.2386007376;
        
        weight_[21] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[21][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[21][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[21][2] = 0.2386007376;
        
        weight_[22] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[22][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[22][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[22][2] = 0.2386007376;
        
        weight_[23] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1434587898);
        gp_[23][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[23][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.2386007376));
        gp_[23][2] = 0.2386007376;
        
        weight_[24] = (98. / 405.) * (0.6863388717e-1);
        gp_[24][0] = (+(std::sqrt(6. / 7.))) * (1. - (0.5170472951));
        gp_[24][1] = (0.) * (1. - (0.5170472951));
        gp_[24][2] = 0.5170472951;
        
        weight_[25] = (98. / 405.) * (0.6863388717e-1);
        gp_[25][0] = (-(std::sqrt(6. / 7.))) * (1. - (0.5170472951));
        gp_[25][1] = (0.) * (1. - (0.5170472951));
        gp_[25][2] = 0.5170472951;
        
        weight_[26] = (98. / 405.) * (0.6863388717e-1);
        gp_[26][0] = (0.) * (1. - (0.5170472951));
        gp_[26][1] = (+(std::sqrt(6. / 7.))) * (1. - (0.5170472951));
        gp_[26][2] = 0.5170472951;
        
        weight_[27] = (98. / 405.) * (0.6863388717e-1);
        gp_[27][0] = (0.) * (1. - (0.5170472951));
        gp_[27][1] = (-(std::sqrt(6. / 7.))) * (1. - (0.5170472951));
        gp_[27][2] = 0.5170472951;
        
        weight_[28] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[28][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[28][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[28][2] = 0.5170472951;
        
        weight_[29] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[29][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[29][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[29][2] = 0.5170472951;
        
        weight_[30] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[30][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[30][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[30][2] = 0.5170472951;
        
        weight_[31] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[31][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[31][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[31][2] = 0.5170472951;
        
        weight_[32] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[32][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[32][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[32][2] = 0.5170472951;
        
        weight_[33] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[33][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[33][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[33][2] = 0.5170472951;
        
        weight_[34] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[34][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[34][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[34][2] = 0.5170472951;
        
        weight_[35] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.6863388717e-1);
        gp_[35][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[35][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.5170472951));
        gp_[35][2] = 0.5170472951;
        
        weight_[36] = (98. / 405.) * (0.1035224075e-1);
        gp_[36][0] = (+(std::sqrt(6. / 7.))) * (1. - (0.7958514179));
        gp_[36][1] = (0.) * (1. - (0.7958514179));
        gp_[36][2] = 0.7958514179;
        
        weight_[37] = (98. / 405.) * (0.1035224075e-1);
        gp_[37][0] = (-(std::sqrt(6. / 7.))) * (1. - (0.7958514179));
        gp_[37][1] = (0.) * (1. - (0.7958514179));
        gp_[37][2] = 0.7958514179;
        
        weight_[38] = (98. / 405.) * (0.1035224075e-1);
        gp_[38][0] = (0.) * (1. - (0.7958514179));
        gp_[38][1] = (+(std::sqrt(6. / 7.))) * (1. - (0.7958514179));
        gp_[38][2] = 0.7958514179;
        
        weight_[39] = (98. / 405.) * (0.1035224075e-1);
        gp_[39][0] = (0.) * (1. - (0.7958514179));
        gp_[39][1] = (-(std::sqrt(6. / 7.))) * (1. - (0.7958514179));
        gp_[39][2] = 0.7958514179;
        
        weight_[40] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[40][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[40][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[40][2] = 0.7958514179;
        
        weight_[41] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[41][0] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[41][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[41][2] = 0.7958514179;
        
        weight_[42] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[42][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[42][1] = (+(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[42][2] = 0.7958514179;
        
        weight_[43] = ((178981. + 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[43][0] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[43][1] = (-(std::sqrt((114. - 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[43][2] = 0.7958514179;
        
        weight_[44] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[44][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[44][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[44][2] = 0.7958514179;
        
        weight_[45] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[45][0] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[45][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[45][2] = 0.7958514179;
        
        weight_[46] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[46][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[46][1] = (+(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[46][2] = 0.7958514179;
        
        weight_[47] = ((178981. - 2769. * std::sqrt(583.)) / 472230.) * (0.1035224075e-1);
        gp_[47][0] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[47][1] = (-(std::sqrt((114. + 3. * std::sqrt(583.)) / 287.))) * (1. - (0.7958514179));
        gp_[47][2] = 0.7958514179;
        
    }
    
    Pyramid_7_1::~Pyramid_7_1()
    {
    }

//---------------------------------------------------------------------------
}// close namespace IntegrationRules

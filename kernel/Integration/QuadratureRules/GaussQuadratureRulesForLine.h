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
#ifndef GaussQuadratureRulesForLine_h
#define GaussQuadratureRulesForLine_h
//---------------------------------------------------------------------------
#include "Integration/QuadratureRules/GaussQuadratureRule.h"
#include <vector>

//---------------------------------------------------------------------------
namespace QuadratureRules
{
    using Geometry::PointReference;
    using Geometry::ReferenceGeometry;
    
//---------------------------------------------------------------------------
    class Cn1_1_1 : public GaussQuadratureRule
    {
    public:
        using PointReferenceT = PointReference;
        using ReferenceGeometryT = ReferenceGeometry;
    public:
        static Cn1_1_1& Instance()
        {
            static Cn1_1_1 theInstance;
            return theInstance;
        }
        
        std::string getName() const override final;
        std::size_t order() const override final;
        std::size_t dimension() const override final;
        std::size_t nrOfPoints() const override final;
        double weight(std::size_t i) const override final;
        const PointReferenceT& getPoint(std::size_t i) const override final;
        ReferenceGeometryT* forReferenceGeometry() const override final;

    private:
        Cn1_1_1();
        Cn1_1_1(const Cn1_1_1&);
    private:
        const std::string name_;
        double weight_[1];
        ReferenceGeometryT* const refGeoPtr_;
        std::vector<PointReferenceT> gp_;
    };
    
//---------------------------------------------------------------------------
    class Cn1_3_2 : public GaussQuadratureRule
    {
    public:
        using PointReferenceT = PointReference;
        using ReferenceGeometryT = ReferenceGeometry;
    public:
        static Cn1_3_2& Instance()
        {
            static Cn1_3_2 theInstance;
            return theInstance;
        }
        
        std::string getName() const override final;
        std::size_t order() const override final;
        std::size_t dimension() const override final;
        std::size_t nrOfPoints() const override final;
        double weight(std::size_t i) const override final;
        const PointReferenceT& getPoint(std::size_t i) const override final;
        ReferenceGeometryT* forReferenceGeometry() const override final;

    private:
        Cn1_3_2();
        Cn1_3_2(const Cn1_3_2&);
    private:
        const std::string name_;
        double weight_[2];
        ReferenceGeometryT* const refGeoPtr_;
        std::vector<PointReferenceT> gp_;
    };
    
//---------------------------------------------------------------------------
    class Cn1_5_3 : public GaussQuadratureRule
    {
    public:
        using PointReferenceT = PointReference;
        using ReferenceGeometryT = ReferenceGeometry;
    public:
        static Cn1_5_3& Instance()
        {
            static Cn1_5_3 theInstance;
            return theInstance;
        }
        
        std::string getName() const override final;
        std::size_t order() const override final;
        std::size_t dimension() const override final;
        std::size_t nrOfPoints() const override final;
        double weight(std::size_t i) const override final;
        const PointReferenceT& getPoint(std::size_t i) const override final;
        ReferenceGeometryT* forReferenceGeometry() const override final;

    private:
        Cn1_5_3();
        Cn1_5_3(const Cn1_5_3&);

    private:
        const std::string name_;
        double weight_[3];
        ReferenceGeometryT* const refGeoPtr_;
        std::vector<PointReferenceT> gp_;
    };
    
//---------------------------------------------------------------------------
    class C1_7_4 : public GaussQuadratureRule
    {
    public:
        using PointReferenceT = PointReference;
        using ReferenceGeometryT = ReferenceGeometry;
    public:
        static C1_7_4& Instance()
        {
            static C1_7_4 theInstance;
            return theInstance;
        }
        
        std::string getName() const override final;
        std::size_t order() const override final;
        std::size_t dimension() const override final;
        std::size_t nrOfPoints() const override final;
        double weight(std::size_t i) const override final;
        const PointReferenceT& getPoint(std::size_t i) const override final;
        ReferenceGeometryT* forReferenceGeometry() const override final;

    private:
        C1_7_4();
        C1_7_4(const C1_7_4&);
    private:
        const std::string name_;
        double weight_[4];
        ReferenceGeometryT* const refGeoPtr_;
        std::vector<PointReferenceT> gp_;
    };
    
//---------------------------------------------------------------------------
    class C1_9_5 : public GaussQuadratureRule 
    {
    public:
        using PointReferenceT = PointReference;
        using ReferenceGeometryT = ReferenceGeometry;
    public:
        static C1_9_5& Instance()
        {
            static C1_9_5 theInstance;
            return theInstance;
        }
        
        std::string getName() const override final;
        std::size_t order() const override final;
        std::size_t dimension() const override final;
        std::size_t nrOfPoints() const override final;
        double weight(std::size_t i) const override final;
        const PointReferenceT& getPoint(std::size_t i) const override final;
        ReferenceGeometryT* forReferenceGeometry() const override final;

    private:
        C1_9_5();
        C1_9_5(const C1_9_5&);
    private:
        const std::string name_;
        double weight_[5];
        ReferenceGeometryT* const refGeoPtr_;
        std::vector<PointReferenceT> gp_;
    };
    
//---------------------------------------------------------------------------
    class C1_11_6 : public GaussQuadratureRule
    {
    public:
        using PointReferenceT = PointReference;
        using ReferenceGeometryT = ReferenceGeometry;
    public:
        static C1_11_6& Instance()
        {
            static C1_11_6 theInstance;
            return theInstance;
        }
        
        std::string getName() const override final;
        std::size_t order() const override final;
        std::size_t dimension() const override final;
        std::size_t nrOfPoints() const override final;
        double weight(std::size_t i) const override final;
        const PointReferenceT& getPoint(std::size_t i) const override final;
        ReferenceGeometryT* forReferenceGeometry() const override final;

    private:
        C1_11_6();
        C1_11_6(const C1_11_6&);
    private:
        const std::string name_;
        double weight_[6];
        ReferenceGeometryT* const refGeoPtr_;
        std::vector<PointReferenceT> gp_;
    };

//---------------------------------------------------------------------------
}// close namespace QuadratureRules
#endif
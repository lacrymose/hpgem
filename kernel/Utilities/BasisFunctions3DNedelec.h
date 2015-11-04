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

#ifndef BASISFUNCTIONS3DNEDELEC_H_
#define BASISFUNCTIONS3DNEDELEC_H_

#include "Base/BaseBasisFunction.h"
#include <vector>

namespace Base
{
    class BasisFunctionSet;
}

namespace Geometry
{
    template<std::size_t DIM>
    class PointReference;
}

namespace Utilities
{
    
    //! Curl conforming Nedelec edge functions.
    class BasisCurlEdgeNedelec : public Base::BaseBasisFunction
    {
    public:
        BasisCurlEdgeNedelec(int degree1,int degree2,int localFirstVertex,int localSecondVertex);
        
	void eval(const Geometry::PointReference<3>& p, LinearAlgebra::SmallVector<3>& ret) const override;

        LinearAlgebra::SmallVector<3> evalCurl(const Geometry::PointReference<3>& p) const override;
        
    private:
        const int deg1,deg2,i,j;
    };
    
    //! Curl conforming Nedelec face functions.
    class BasisCurlFace1Nedelec : public Base::BaseBasisFunction
    {
    public:
        BasisCurlFace1Nedelec(int degree1,int degree2,int degree3,int localOpposingVertex);
        
        void eval(const Geometry::PointReference<3>& p, LinearAlgebra::SmallVector<3>& ret) const override;

        LinearAlgebra::SmallVector<3> evalCurl(const Geometry::PointReference<3>& p) const override;
        
    private:
        int deg1,deg2,deg3,a,b,c,d;
    };

    class BasisCurlFace2Nedelec : public Base::BaseBasisFunction
    {
    public:
        BasisCurlFace2Nedelec(int degree1,int degree2,int degree3,int localOpposingVertex);
        
        void eval(const Geometry::PointReference<3>& p, LinearAlgebra::SmallVector<3>& ret) const override;

        LinearAlgebra::SmallVector<3> evalCurl(const Geometry::PointReference<3>& p) const override;
        
    private:
        int deg1,deg2,deg3,a,b,c,d;
    };
    
    //! curl conforming interior Nedelec functions
    class BasisCurlinterior1Nedelec : public Base::BaseBasisFunction
    {
    public:
        BasisCurlinterior1Nedelec(int degree1,int degree2,int degree3,int degree4);
                
        void eval(const Geometry::PointReference<3>& p, LinearAlgebra::SmallVector<3>& ret) const override;

        LinearAlgebra::SmallVector<3> evalCurl(const Geometry::PointReference<3>& p) const override;
        
    private:
        const int deg1,deg2,deg3,deg4;
    };

    //! curl conforming interior Nedelec functions
    class BasisCurlinterior2Nedelec : public Base::BaseBasisFunction
    {
    public:
        BasisCurlinterior2Nedelec(int degree1,int degree2,int degree3,int degree4);
                
        void eval(const Geometry::PointReference<3>& p, LinearAlgebra::SmallVector<3>& ret) const override;

        LinearAlgebra::SmallVector<3> evalCurl(const Geometry::PointReference<3>& p) const override;
        
    private:
        const int deg1,deg2,deg3,deg4;
    };

    //! curl conforming interior Nedelec functions
    class BasisCurlinterior3Nedelec : public Base::BaseBasisFunction
    {
    public:
        BasisCurlinterior3Nedelec(int degree1,int degree2,int degree3,int degree4);
                
        void eval(const Geometry::PointReference<3>& p, LinearAlgebra::SmallVector<3>& ret) const override;

        LinearAlgebra::SmallVector<3> evalCurl(const Geometry::PointReference<3>& p) const override;
        
    private:
        const int deg1,deg2,deg3,deg4;
    };

    Base::BasisFunctionSet* createDGBasisFunctionSet3DNedelec(std::size_t order);

}

#endif


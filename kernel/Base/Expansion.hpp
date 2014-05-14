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

#ifndef Expansion_hpp
#define Expansion_hpp

#include "../LinearAlgebra/NumericalVector.hpp"
#include "../Geometry/PointReference.hpp"
#include "../Base/BasisFunctionSet.hpp"
#include "../LinearAlgebra/Matrix.hpp"
namespace Base
{
	///\TODO where is this used? -FB
    class Expansion
    {

    public:

        typedef BasisFunctionSet::PointReferenceT PointReferenceT;
        Expansion(const BasisFunctionSet* BFSetPtr) :
            BFSetPtr_(BFSetPtr),
            coeff_(BFSetPtr->size()),
            size_(BFSetPtr->size())
        {}

        ~Expansion() {};

        void setBasisFunction(const BasisFunctionSet* BFSetPtr)
        {
            BFSetPtr_ = BFSetPtr;
            size_ = BFSetPtr_->size();
            coeff_.resize(size_);
        }

        double EvalBasisFunction(unsigned int iBF, const PointReferenceT& p) const
        {
            return BFSetPtr_->eval(iBF,p);
        }

        double EvalDerivBasisFunction(unsigned int iBF, const unsigned int jDir, const PointReferenceT& p) const
        {
            return BFSetPtr_->evalDeriv(iBF,jDir,p);
        }

        void PhysGradientOfBasisFunction(
                const Element& el,
                const unsigned int iBF,
                const PointReferenceT& p,
                LinearAlgebra::NumericalVector& ret) const
        {
            ret.resize(p.size());
            // get derivatives df
            // get Jacobian    jac
            // solve the linear system and return
        }

	double Eval(const PointReferenceT& p) const
	{
	  double ret(0.);
	  for (unsigned int i=0; i<size_; ++i)
	    ret += (coeff_[i] * BFSetPtr_->eval(i,p));

	  return ret;
	}

	double EvalDeriv(const unsigned int jDir, const PointReferenceT& p) const
	{
	  double ret(0.);
	  for (unsigned int i=0; i<size_; ++i)
	    ret += (coeff_[i] * BFSetPtr_->evalDeriv(i,jDir,p));

	  return ret;
	}

      private:
	BasisFunctionSet* BFSetPtr_;
	LinearAlgebra::NumericalVector coeff_;
	unsigned int size_;
    };

};

#endif // Expansion_hpp

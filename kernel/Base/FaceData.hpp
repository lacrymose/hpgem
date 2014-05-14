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
#ifndef FaceData_hpp
#define FaceData_hpp
//----------------------------------------------------------------
#include <vector>
#include "FaceCacheData.hpp"
#include "LinearAlgebra/Matrix.hpp"
#include "UserData.hpp"

namespace Base {
	class FaceData {
	public:
		typedef FaceCacheData CacheT;
		typedef std::vector<CacheT> VecCacheT;

	public:
		FaceData(unsigned int numberOfDOF, unsigned int numberOfFaceMatrices = 0, unsigned int numberOfFaceVactors = 0);

		void setFaceMatrix(const LinearAlgebra::Matrix& matrix, unsigned int matrixID = 0);

		virtual void getFaceMatrix(LinearAlgebra::Matrix& matrix, unsigned int matrixID = 0) const;

		void setFaceVector(const LinearAlgebra::NumericalVector& vector, unsigned int vectorID = 0);

		virtual void getFaceVector(LinearAlgebra::NumericalVector& vector, unsigned int vectorID = 0) const;

		virtual const VecCacheT& getVecCacheData() const {
			return vecCacheData_;
		}

		virtual ~FaceData() {
			;
		}

		virtual UserFaceData* getUserData() const {
			return userData_;
		}

		void setUserData(UserFaceData* data) {
			userData_ = data;
		}

	private:
		VecCacheT vecCacheData_;
		UserFaceData* userData_;
		std::vector<LinearAlgebra::Matrix> faceMatrix_;
		std::vector<LinearAlgebra::NumericalVector> faceVector_;
	};
}
;
#endif

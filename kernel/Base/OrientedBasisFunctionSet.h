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

#ifndef ORIENTEDBASISFUNCTIONSET_HPP_
#define ORIENTEDBASISFUNCTIONSET_HPP_

#include "BasisFunctionSet.h"

namespace Base
{
    
    /**
     * has all the functionality of a basisfunctionset, but is meant for cases
     * where the orientation of the basisfunctions matter
     *
     * it is assumed that this set contains only part of the basis (for example only the functions with their DOF linked to a face)
     */
    class OrientedBasisFunctionSet : public Base::BasisFunctionSet
    {
    public:
        OrientedBasisFunctionSet(std::size_t order, std::size_t orientation, std::size_t face)
                : BasisFunctionSet(order), orientation_(orientation), face_(face)
        {
        }

        bool checkOrientation(std::size_t codim0mapIndex, std::size_t faceIndex) const
        {
            return codim0mapIndex == orientation_ && faceIndex == face_;
        }
    private:
        std::size_t orientation_;
        std::size_t face_;
    };

} /* namespace Base */

#endif /* ORIENTEDBASISFUNCTIONSET_HPP_ */
/*
 This file forms part of hpGEM. This package has been developed over a number of
 years by various people at the University of Twente and a full list of
 contributors can be found at http://hpgem.org/about-the-code/team

 This code is distributed using BSD 3-Clause License. A copy of which can found
 below.


 Copyright (c) 2017, University of Twente
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HPGEM_APP_HPGEM_H
#define HPGEM_APP_HPGEM_H

#include "customIterator.h"
#include <vector>
#include <string>
#include <fstream>

using namespace hpgem;


namespace Preprocessor {

class PrivateReader {
   public:
    virtual ~PrivateReader() = default;
    PrivateReader(const PrivateReader&) = delete;
    PrivateReader(PrivateReader&&) = delete;
    PrivateReader& operator=(const PrivateReader&) = delete;
    PrivateReader& operator=(PrivateReader&&) = delete;

    virtual Range<std::vector<std::vector<double>>> getNodeCoordinates() = 0;
    virtual Range<std::vector<std::size_t>> getElements() = 0;
    virtual std::size_t getDimension() = 0;
    virtual std::size_t getTargetProcessorCount() = 0;
    virtual Range<std::size_t> getProcessorBindings() = 0;

   protected:
    PrivateReader() = default;
};

class HpgemReader {
   public:
    HpgemReader(std::string filename);

    Range<std::vector<std::vector<double>>> getNodeCoordinates() {
        return impl->getNodeCoordinates();
    }
    Range<std::vector<std::size_t>> getElements() {
        return impl->getElements();
    }
    std::size_t getDimension() { return impl->getDimension(); }
    std::size_t getTargetProcessorCount() {
        return impl->getTargetProcessorCount();
    }
    Range<std::size_t> getProcessorBindings() {
        return impl->getProcessorBindings();
    }

   private:
    std::shared_ptr<PrivateReader> impl{nullptr};
};
}  // namespace Preprocessor

#endif  // HPGEM_APP_HPGEM_H

/*
 This file forms part of hpGEM. This package has been developed over a number of
 years by various people at the University of Twente and a full list of
 contributors can be found at http://hpgem.org/about-the-code/team

 This code is distributed using BSD 3-Clause License. A copy of which can found
 below.


 Copyright (c) 2014, University of Twente
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

#ifdef HPGEM_USE_QHULL
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullQh.h"
#include "libqhullcpp/RboxPoints.h"
#include "libqhullcpp/QhullError.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullFacetSet.h"
#include "libqhullcpp/QhullLinkedList.h"
#include "libqhullcpp/QhullVertex.h"
#include "libqhullcpp/QhullVertexSet.h"
#include "libqhullcpp/Qhull.h"
#endif

#include "MeshManipulator.h"

#include "Geometry/PhysicalGeometry.h"
#include "Geometry/ReferenceTriangle.h"
#include "Edge.h"
#include "Base/BasisFunctionSet.h"
#include "ConfigurationData.h"
#include "Element.h"
#include "PhysicalElement.h"
#include "Face.h"
#include "MeshMoverBase.h"
#include "OrientedBasisFunctionSet.h"
#include "Geometry/PointPhysical.h"
#include "Geometry/ReferenceSquare.h"
#include "Geometry/GlobalNamespaceGeometry.h"
#include "Geometry/PointReference.h"
#include "BaseBasisFunction.h"
#include "Geometry/Mappings/MappingReferenceToPhysical.h"
#include "ElementFactory.h"
#include "FaceFactory.h"
#include "L2Norm.h"
#include "Geometry/Jacobian.h"
#include "Geometry/ReferenceGeometry.h"
#include "Utilities/BasisFunctions1DH1ConformingLine.h"
#include "Utilities/BasisFunctions2DH1ConformingSquare.h"
#include "Utilities/BasisFunctions2DH1ConformingTriangle.h"
#include "Utilities/BasisFunctions3DH1ConformingCube.h"
#include "Utilities/BasisFunctions3DH1ConformingPrism.h"
#include "Utilities/BasisFunctions3DH1ConformingTetrahedron.h"
#include "Logger.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <array>
#include <numeric>
#include <type_traits>
#include <typeinfo>

namespace hpgem {

namespace Base {

MeshManipulatorBase::MeshManipulatorBase(const ConfigurationData* config,
                                         std::size_t dimension,
                                         std::size_t numberOfElementMatrices,
                                         std::size_t numberOfElementVectors,
                                         std::size_t numberOfFaceMatrtices,
                                         std::size_t numberOfFaceVectors)
    : configData_(config),
      numberOfElementMatrices_(numberOfElementMatrices),
      numberOfFaceMatrices_(numberOfFaceMatrtices),
      numberOfElementVectors_(numberOfElementVectors),
      numberOfFaceVectors_(numberOfFaceVectors),
      dimension_(dimension) {
    logger.assert_debug(config != nullptr, "Invalid configuration passed");
    logger(INFO, "******Mesh creation started!**************");
    logger(VERBOSE, "numberOfElementVectors = %", numberOfElementVectors);
    logger(VERBOSE, "numberOfFaceVectors = %", numberOfFaceVectors);
}

MeshManipulatorBase::MeshManipulatorBase(const MeshManipulatorBase& other) =
    default;
}  // namespace Base

}  // namespace hpgem

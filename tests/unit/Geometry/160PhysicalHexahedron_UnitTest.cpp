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

// naming convention: <Digit><ClassName>_UnitTest.cpp where <Digit> is a number
// that will make sure the unit tests are ordered such that the first failing
// unit test indicate the culprit class and other 'unit' tests may assume
// correct execution of all prior unit tests
#include "Geometry/PhysicalHexahedron.h"
#include "Logger.h"
#include "Geometry/PointPhysical.h"
#include "Geometry/PointReference.h"
#include "Geometry/ReferenceCube.h"

#include <cmath>

#include "../catch.hpp"

using namespace hpgem;
using Geometry::PhysicalHexahedron;

TEST_CASE("160PhysicalHexahedron_UnitTest",
          "[160PhysicalHexahedron_UnitTest]") {

    std::vector<std::size_t> pointIndexes;
    std::vector<Geometry::PointPhysical<3> > nodes;

    Geometry::PointPhysical<3> point;

    pointIndexes.push_back(4);
    pointIndexes.push_back(7);
    pointIndexes.push_back(10);
    pointIndexes.push_back(11);
    pointIndexes.push_back(12);
    pointIndexes.push_back(13);
    pointIndexes.push_back(14);
    pointIndexes.push_back(15);

    for (double i = 0.; i < 1 - 1e-10; i += 0.1) {
        point[0] = 1. + i;
        point[1] = 2. + i;
        point[2] = 3. + i;
        nodes.push_back(point);
    }

    point[0] = 3.5;
    point[1] = 4.6;
    point[2] = 5.4;
    nodes.push_back(point);
    point[0] = 6.7;
    point[1] = 2.8;
    point[2] = 5.7;
    nodes.push_back(point);
    point[0] = 1.4;
    point[1] = 2.4;
    point[2] = 5.4;
    nodes.push_back(point);
    point[0] = 1.7;
    point[1] = 2.7;
    point[2] = 5.7;
    nodes.push_back(point);
    point[0] = 3.5;
    point[1] = 4.6;
    point[2] = 7.4;
    nodes.push_back(point);
    point[0] = 6.7;
    point[1] = 2.8;
    point[2] = 7.7;
    nodes.push_back(point);

    PhysicalHexahedron test(pointIndexes, nodes);

    std::cout << test;

    pointIndexes = test.getNodeIndexes();

    INFO("getNodeIndexes");
    CHECK((pointIndexes[0] == 4 && pointIndexes[1] == 7 &&
           pointIndexes[2] == 10 && pointIndexes[3] == 11 &&
           pointIndexes[4] == 12 && pointIndexes[5] == 13 &&
           pointIndexes[6] == 14 && pointIndexes[7] == 15));
    INFO("getNodes");
    CHECK((nodes == test.getNodeCoordinates()));
    INFO("getNodeIndex");
    CHECK((test.getNodeIndex(0) == 4 && test.getNodeIndex(1) == 7 &&
           test.getNodeIndex(2) == 10 && test.getNodeIndex(3) == 11 &&
           test.getNodeIndex(4) == 12 && test.getNodeIndex(5) == 13 &&
           test.getNodeIndex(6) == 14 && test.getNodeIndex(7) == 15));

    std::cout << test.getName();

    point = *test.getNodeCoordinatePtr(test.getNodeIndex(0));
    INFO("getNodePtr");
    CHECK((std::abs(point[0] - 1.4) < 1e-12));
    INFO("getNodePtr");
    CHECK((std::abs(point[1] - 2.4) < 1e-12));
    INFO("getNodePtr");
    CHECK((std::abs(point[2] - 3.4) < 1e-12));
    point = *test.getNodeCoordinatePtr(test.getNodeIndex(1));
    INFO("getNodePtr");
    CHECK((std::abs(point[0] - 1.7) < 1e-12));
    INFO("getNodePtr");
    CHECK((std::abs(point[1] - 2.7) < 1e-12));
    INFO("getNodePtr");
    CHECK((std::abs(point[2] - 3.7) < 1e-12));

    INFO("getNumberOfNodes");
    CHECK((test.getNumberOfNodes() == 8));

    point = test.getLocalNodeCoordinates(0);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 1.4) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 2.4) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 3.4) < 1e-12));
    point = test.getLocalNodeCoordinates(1);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 1.7) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 2.7) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 3.7) < 1e-12));
    point = test.getLocalNodeCoordinates(2);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 3.5) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 4.6) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 5.4) < 1e-12));
    point = test.getLocalNodeCoordinates(3);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 6.7) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 2.8) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 5.7) < 1e-12));
    point = test.getLocalNodeCoordinates(4);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 1.4) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 2.4) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 5.4) < 1e-12));
    point = test.getLocalNodeCoordinates(5);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 1.7) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 2.7) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 5.7) < 1e-12));
    point = test.getLocalNodeCoordinates(6);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 3.5) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 4.6) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 7.4) < 1e-12));
    point = test.getLocalNodeCoordinates(7);
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[0] - 6.7) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[1] - 2.8) < 1e-12));
    INFO("getLocalNodeCoordinates");
    CHECK((std::abs(point[2] - 7.7) < 1e-12));

    for (std::size_t i = 0; i < 10; ++i) {
        point = test.getGlobalNodeCoordinates(i);
        INFO("getGlobalNodeCoordinates");
        CHECK((std::abs(point[0] - 1. - i / 10.) < 1e-12));
        INFO("getGlobalNodeCoordinates");
        CHECK((std::abs(point[1] - 2. - i / 10.) < 1e-12));
        INFO("getGlobalNodeCoordinates");
        CHECK((std::abs(point[2] - 3. - i / 10.) < 1e-12));
    }

    pointIndexes.resize(4);

    for (std::size_t i = 0; i < 6; ++i) {
        pointIndexes = test.getGlobalFaceNodeIndices(i);
        for (std::size_t j = 0; j < 4; ++j) {
            INFO("getGlobalFaceNodeIndices");
            CHECK((pointIndexes[j] ==
                   test.getNodeIndex(
                       test.getRefGeometry()
                           ->getLocalNodeIndexFromFaceAndIndexOnFace(i, j))));
        }
    }

    for (std::size_t i = 0; i < 6; ++i) {
        pointIndexes = test.getLocalFaceNodeIndices(i);
        for (std::size_t j = 0; j < 4; ++j) {
            INFO("getLocalFaceNodeIndices");
            CHECK(
                (pointIndexes[j] ==
                 test.getRefGeometry()->getLocalNodeIndexFromFaceAndIndexOnFace(
                     i, j)));
        }
    }

    INFO("getNumberOfFaces");
    CHECK((test.getNumberOfFaces() == 6));

    INFO("getRefGeometry");
    CHECK((test.getRefGeometry() == &Geometry::ReferenceCube::Instance()));
}

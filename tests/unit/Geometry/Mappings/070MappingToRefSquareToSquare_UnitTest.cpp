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

//naming convention: <Digit><ClassName>_UnitTest.cpp where <Digit> is a number that will make sure
//the unit tests are ordered such that the first failing unit test indicate the culprit class and
//other 'unit' tests may assume correct execution of all prior unit tests
#include "Geometry/Mappings/MappingToRefSquareToSquare.h"
#include <Logger.h>

#include "Geometry/ReferenceSquare.h"
#include "Geometry/PointReference.h"
#include "Geometry/Jacobian.h"
#include "LinearAlgebra/NumericalVector.h"
#include <cmath>
int main()
{
    
    Geometry::Point refPoint(2), point(2), compare(2);
    
    Geometry::ReferenceSquare& geom = Geometry::ReferenceSquare::Instance();
    
    Geometry::Jacobian jac(2, 2);
    
    std::vector<std::size_t> nodesAfterTransformation(4);
    
    const Geometry::MappingReferenceToReference* test = &Geometry::MappingToRefSquareToSquare0::Instance();
    nodesAfterTransformation[0] = 0;
    nodesAfterTransformation[1] = 1;
    nodesAfterTransformation[2] = 2;
    nodesAfterTransformation[3] = 3;
    
    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    test = &Geometry::MappingToRefSquareToSquare1::Instance();
    nodesAfterTransformation[0] = 1;
    nodesAfterTransformation[1] = 3;
    nodesAfterTransformation[2] = 0;
    nodesAfterTransformation[3] = 2;

    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    test = &Geometry::MappingToRefSquareToSquare2::Instance();
    nodesAfterTransformation[0] = 3;
    nodesAfterTransformation[1] = 2;
    nodesAfterTransformation[2] = 1;
    nodesAfterTransformation[3] = 0;

    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    test = &Geometry::MappingToRefSquareToSquare3::Instance();
    nodesAfterTransformation[0] = 2;
    nodesAfterTransformation[1] = 0;
    nodesAfterTransformation[2] = 3;
    nodesAfterTransformation[3] = 1;

    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    test = &Geometry::MappingToRefSquareToSquare4::Instance();
    nodesAfterTransformation[0] = 2;
    nodesAfterTransformation[1] = 3;
    nodesAfterTransformation[2] = 0;
    nodesAfterTransformation[3] = 1;

    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    test = &Geometry::MappingToRefSquareToSquare5::Instance();
    nodesAfterTransformation[0] = 1;
    nodesAfterTransformation[1] = 0;
    nodesAfterTransformation[2] = 3;
    nodesAfterTransformation[3] = 2;

    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    test = &Geometry::MappingToRefSquareToSquare6::Instance();
    nodesAfterTransformation[0] = 3;
    nodesAfterTransformation[1] = 1;
    nodesAfterTransformation[2] = 2;
    nodesAfterTransformation[3] = 0;

    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    test = &Geometry::MappingToRefSquareToSquare7::Instance();
    nodesAfterTransformation[0] = 0;
    nodesAfterTransformation[1] = 2;
    nodesAfterTransformation[2] = 1;
    nodesAfterTransformation[3] = 3;

    for (refPoint[0] = -1.51; refPoint[0] < 1.51; refPoint[0] += 0.2)
    {
        for (refPoint[1] = -1.51; refPoint[1] < 1.51; refPoint[1] += 0.2)
        {
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint)) == geom.isInternalPoint(*Geometry::PointReferenceFactory::instance()->makePoint(point))), "transform");
            
            refPoint[0] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[0] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[0] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[0] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian"); //estimate is a bit rough, but should work for most mappings
            logger.assert_always((std::abs(jac[1] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian"); //implementations are very strongly recommended to be more accurate
                    
            refPoint[1] += -1.e-8;
            compare = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            refPoint[1] += 2.e-8;
            point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            
            refPoint[1] += -1e-8;
            jac = test->calcJacobian(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
            logger.assert_always((std::abs(jac[2] - 5.e7 * (point[0] - compare[0])) < 1e-5), "jacobian");
            logger.assert_always((std::abs(jac[3] - 5.e7 * (point[1] - compare[1])) < 1e-5), "jacobian");
        }
    }
    
    for (std::size_t i = 0; i < geom.getNumberOfNodes(); ++i)
    {
        refPoint = geom.getNode(i);
        compare = geom.getNode(nodesAfterTransformation[i]);
        point = test->transform(*Geometry::PointReferenceFactory::instance()->makePoint(refPoint));
        logger.assert_always((std::abs(point[0] - compare[0]) < 1e-12), "transform");
        logger.assert_always((std::abs(point[1] - compare[1]) < 1e-12), "transform");
    }
    
    logger.assert_always((test->getTargetDimension() == 2), "getTargetDimension");
    
    return 0;
}

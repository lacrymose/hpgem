/*
 * 160MappingToRefFaceToPyramid_UnitTest.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: brinkf
 */

//naming convention: <Digit><ClassName>_UnitTest.cpp where <Digit> is a number that will make sure
//the unit tests are ordered such that the first failing unit test indicate the culprit class and
//other 'unit' tests may assume correct execution of all prior unit tests

#include "Geometry/Mappings/MappingToRefFaceToPyramid.hpp"
#include "cassert"

#include "Geometry/ReferencePyramid.hpp"
#include "Geometry/ReferenceTriangle.hpp"
#include "Geometry/ReferenceSquare.hpp"
int main() {

	Geometry::PointReference refPoint(2),point(3),compare(3);

	Geometry::ReferencePyramid& eGeom = Geometry::ReferencePyramid::Instance();
	Geometry::ReferenceGeometry* fGeom = &Geometry::ReferenceSquare::Instance();

	Geometry::Jacobian jac(2,3);

	std::vector<int> nodesAfterTransformation(4);

	const Geometry::MappingReferenceToReference* test = &Geometry::MappingToRefFaceToPyramid0::Instance();
	nodesAfterTransformation[0]=3;
	nodesAfterTransformation[1]=4;
	nodesAfterTransformation[2]=1;
	nodesAfterTransformation[3]=2;

	for(refPoint[0]=-2.8189;refPoint[0]<3.141;refPoint[0]+=0.1) {
		for(refPoint[1]=-2.8189;refPoint[1]<3.141;refPoint[1]+=0.1) {
			test->transform(refPoint,point);
			assert(("transform",fGeom->isInternalPoint(refPoint)==eGeom.isInternalPoint(point)));

			refPoint[0]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[0]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[0]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[0]-5.e7*(point[0]-compare[0]))<1e-5));//estimate is a bit rough, but should work for most mappings
			assert(("jacobian",fabs(jac[1]-5.e7*(point[1]-compare[1]))<1e-5));//implementations are very strongly recommended to be more accurate
			assert(("jacobian",fabs(jac[2]-5.e7*(point[2]-compare[2]))<1e-5));

			refPoint[1]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[1]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[1]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[3]-5.e7*(point[0]-compare[0]))<1e-5));
			assert(("jacobian",fabs(jac[4]-5.e7*(point[1]-compare[1]))<1e-5));
			assert(("jacobian",fabs(jac[5]-5.e7*(point[2]-compare[2]))<1e-5));
		}
	}

	for(int i=0;i<fGeom->getNumberOfNodes();++i){
		fGeom->getNode(i,refPoint);
		eGeom.getNode(nodesAfterTransformation[i],compare);
		test->transform(refPoint,point);
		assert(("transform",fabs(point[0]-compare[0])<1e-12));
		assert(("transform",fabs(point[1]-compare[1])<1e-12));
		assert(("transform",fabs(point[2]-compare[2])<1e-12));
	}

	assert(("getTargetDimension",test->getTargetDimension()==3));



	fGeom = &Geometry::ReferenceTriangle::Instance();
	nodesAfterTransformation.resize(3);

	test = &Geometry::MappingToRefFaceToPyramid1::Instance();
	nodesAfterTransformation[0]=3;
	nodesAfterTransformation[1]=1;
	nodesAfterTransformation[2]=0;

	for(refPoint[0]=-2.8189;refPoint[0]<3.141;refPoint[0]+=0.1) {
		for(refPoint[1]=-2.8189;refPoint[1]<3.141;refPoint[1]+=0.1) {
			test->transform(refPoint,point);
			assert(("transform",fGeom->isInternalPoint(refPoint)==eGeom.isInternalPoint(point)));

			refPoint[0]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[0]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[0]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[0]-5.e7*(point[0]-compare[0]))<1e-5));//estimate is a bit rough, but should work for most mappings
			assert(("jacobian",fabs(jac[1]-5.e7*(point[1]-compare[1]))<1e-5));//implementations are very strongly recommended to be more accurate
			assert(("jacobian",fabs(jac[2]-5.e7*(point[2]-compare[2]))<1e-5));

			refPoint[1]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[1]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[1]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[3]-5.e7*(point[0]-compare[0]))<1e-5));
			assert(("jacobian",fabs(jac[4]-5.e7*(point[1]-compare[1]))<1e-5));
			assert(("jacobian",fabs(jac[5]-5.e7*(point[2]-compare[2]))<1e-5));
		}
	}

	for(int i=0;i<fGeom->getNumberOfNodes();++i){
		fGeom->getNode(i,refPoint);
		eGeom.getNode(nodesAfterTransformation[i],compare);
		test->transform(refPoint,point);
		assert(("transform",fabs(point[0]-compare[0])<1e-12));
		assert(("transform",fabs(point[1]-compare[1])<1e-12));
		assert(("transform",fabs(point[2]-compare[2])<1e-12));
	}

	assert(("getTargetDimension",test->getTargetDimension()==3));


	test = &Geometry::MappingToRefFaceToPyramid2::Instance();
	nodesAfterTransformation[0]=2;
	nodesAfterTransformation[1]=4;
	nodesAfterTransformation[2]=0;

	for(refPoint[0]=-2.8189;refPoint[0]<3.141;refPoint[0]+=0.1) {
		for(refPoint[1]=-2.8189;refPoint[1]<3.141;refPoint[1]+=0.1) {
			test->transform(refPoint,point);
			assert(("transform",fGeom->isInternalPoint(refPoint)==eGeom.isInternalPoint(point)));

			refPoint[0]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[0]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[0]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[0]-5.e7*(point[0]-compare[0]))<1e-5));//estimate is a bit rough, but should work for most mappings
			assert(("jacobian",fabs(jac[1]-5.e7*(point[1]-compare[1]))<1e-5));//implementations are very strongly recommended to be more accurate
			assert(("jacobian",fabs(jac[2]-5.e7*(point[2]-compare[2]))<1e-5));

			refPoint[1]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[1]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[1]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[3]-5.e7*(point[0]-compare[0]))<1e-5));
			assert(("jacobian",fabs(jac[4]-5.e7*(point[1]-compare[1]))<1e-5));
			assert(("jacobian",fabs(jac[5]-5.e7*(point[2]-compare[2]))<1e-5));
		}
	}

	for(int i=0;i<fGeom->getNumberOfNodes();++i){
		fGeom->getNode(i,refPoint);
		eGeom.getNode(nodesAfterTransformation[i],compare);
		test->transform(refPoint,point);
		assert(("transform",fabs(point[0]-compare[0])<1e-12));
		assert(("transform",fabs(point[1]-compare[1])<1e-12));
		assert(("transform",fabs(point[2]-compare[2])<1e-12));
	}

	assert(("getTargetDimension",test->getTargetDimension()==3));


	test = &Geometry::MappingToRefFaceToPyramid3::Instance();
	nodesAfterTransformation[0]=1;
	nodesAfterTransformation[1]=2;
	nodesAfterTransformation[2]=0;

	for(refPoint[0]=-2.8189;refPoint[0]<3.141;refPoint[0]+=0.1) {
		for(refPoint[1]=-2.8189;refPoint[1]<3.141;refPoint[1]+=0.1) {
			test->transform(refPoint,point);
			assert(("transform",fGeom->isInternalPoint(refPoint)==eGeom.isInternalPoint(point)));

			refPoint[0]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[0]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[0]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[0]-5.e7*(point[0]-compare[0]))<1e-5));//estimate is a bit rough, but should work for most mappings
			assert(("jacobian",fabs(jac[1]-5.e7*(point[1]-compare[1]))<1e-5));//implementations are very strongly recommended to be more accurate
			assert(("jacobian",fabs(jac[2]-5.e7*(point[2]-compare[2]))<1e-5));

			refPoint[1]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[1]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[1]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[3]-5.e7*(point[0]-compare[0]))<1e-5));
			assert(("jacobian",fabs(jac[4]-5.e7*(point[1]-compare[1]))<1e-5));
			assert(("jacobian",fabs(jac[5]-5.e7*(point[2]-compare[2]))<1e-5));
		}
	}

	for(int i=0;i<fGeom->getNumberOfNodes();++i){
		fGeom->getNode(i,refPoint);
		eGeom.getNode(nodesAfterTransformation[i],compare);
		test->transform(refPoint,point);
		assert(("transform",fabs(point[0]-compare[0])<1e-12));
		assert(("transform",fabs(point[1]-compare[1])<1e-12));
		assert(("transform",fabs(point[2]-compare[2])<1e-12));
	}

	assert(("getTargetDimension",test->getTargetDimension()==3));


	test = &Geometry::MappingToRefFaceToPyramid4::Instance();
	nodesAfterTransformation[0]=4;
	nodesAfterTransformation[1]=3;
	nodesAfterTransformation[2]=0;

	for(refPoint[0]=-2.8189;refPoint[0]<3.141;refPoint[0]+=0.1) {
		for(refPoint[1]=-2.8189;refPoint[1]<3.141;refPoint[1]+=0.1) {
			test->transform(refPoint,point);
			assert(("transform",fGeom->isInternalPoint(refPoint)==eGeom.isInternalPoint(point)));

			refPoint[0]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[0]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[0]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[0]-5.e7*(point[0]-compare[0]))<1e-5));//estimate is a bit rough, but should work for most mappings
			assert(("jacobian",fabs(jac[1]-5.e7*(point[1]-compare[1]))<1e-5));//implementations are very strongly recommended to be more accurate
			assert(("jacobian",fabs(jac[2]-5.e7*(point[2]-compare[2]))<1e-5));

			refPoint[1]+=-1.e-8;
			test->transform(refPoint,compare);
			refPoint[1]+=2.e-8;
			test->transform(refPoint,point);

			refPoint[1]+=-1e-8;
			test->calcJacobian(refPoint,jac);
			assert(("jacobian",fabs(jac[3]-5.e7*(point[0]-compare[0]))<1e-5));
			assert(("jacobian",fabs(jac[4]-5.e7*(point[1]-compare[1]))<1e-5));
			assert(("jacobian",fabs(jac[5]-5.e7*(point[2]-compare[2]))<1e-5));
		}
	}

	for(int i=0;i<fGeom->getNumberOfNodes();++i){
		fGeom->getNode(i,refPoint);
		eGeom.getNode(nodesAfterTransformation[i],compare);
		test->transform(refPoint,point);
		assert(("transform",fabs(point[0]-compare[0])<1e-12));
		assert(("transform",fabs(point[1]-compare[1])<1e-12));
		assert(("transform",fabs(point[2]-compare[2])<1e-12));
	}

	assert(("getTargetDimension",test->getTargetDimension()==3));

	return 0;
}


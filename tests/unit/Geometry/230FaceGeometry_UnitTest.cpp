/*
 * 230FaceGeometry_UnitTest.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: brinkf
 */

//naming convention: <Digit><ClassName>_UnitTest.cpp where <Digit> is a number that will make sure
//the unit tests are ordered such that the first failing unit test indicate the culprit class and
//other 'unit' tests may assume correct execution of all prior unit tests

#include "Geometry/FaceGeometry.hpp"
#include "cassert"

#include "Geometry/ReferenceLine.hpp"

int main(){

	//dim0

	std::vector<unsigned int> pointIndexes,leftIndices,rightIndices;
	std::vector<Geometry::PointPhysical> nodes1D;

	Geometry::PointReference point1D(1),compare1D(1);
	Geometry::PointPhysical point1Dphys(1),compare1Dphys(1);
	Geometry::PointReference orig1D(1);

	Geometry::Jacobian jac(1,1),jaccompare(1,1);

	pointIndexes.push_back(4);
	pointIndexes.push_back(7);

	for(double i=0.;i<10;++i){
		point1D[0]=1.+i/10.;
		nodes1D.push_back(point1D);
	}

	Geometry::ElementGeometry *left(new Geometry::ElementGeometry(pointIndexes,nodes1D));
	pointIndexes[1]=2;
	Geometry::ElementGeometry *right(new Geometry::ElementGeometry(pointIndexes,nodes1D));

	Geometry::FaceGeometry *test(new Geometry::FaceGeometry(left,0,right,0));

	leftIndices.resize(1);
	rightIndices.resize(1);

	///\TODO test the normal vector
	//todo also test individual functions

	test->getElementGLeft()->getPhysicalGeometry()->getLocalFaceNodeIndices(test->localFaceNumberLeft(),leftIndices);
	test->getPtrElementGRight()->getPhysicalGeometry()->getLocalFaceNodeIndices(test->localFaceNumberRight(),rightIndices);

	for(int i=0;i<test->getReferenceGeometry()->getNumberOfNodes();++i){
		test->getReferenceGeometry()->getNode(i,orig1D);
		test->getElementGLeft()->getReferenceGeometry()->getNode(leftIndices[i],compare1D);
		test->getElementGLeft()->referenceToPhysical(compare1D,compare1Dphys);
		test->mapRefFaceToRefElemL(orig1D,point1D);
		test->referenceToPhysical(orig1D,point1Dphys);
		assert(("getElementGLeft or localFaceNumberLeft or mapRefFaceToRefElemL",(compare1D[0]-point1D[0])<1e-12));
		assert(("referenceToPhysical",(compare1Dphys[0]-point1Dphys[0])<1e-12));
		test->getPtrElementGRight()->getReferenceGeometry()->getNode(rightIndices[i],compare1D);
		test->getPtrElementGRight()->referenceToPhysical(compare1D,compare1Dphys);
		test->mapRefFaceToRefElemR(orig1D,point1D);
		assert(("getPtrElementGRight or localFaceNumberRight or mapRefFaceToRefElemR or mapRefFaceToRefFace",(compare1D[0]-point1D[0])<1e-12));
		assert(("referenceToPhysical",(compare1Dphys[0]-point1Dphys[0])<1e-12));//probably indirectly verified already, but this is the most important feature of a face
	}

	assert((test->getFaceType()==Geometry::INTERNAL));
	assert((typeid(*test->getReferenceGeometry())==typeid(Geometry::ReferencePoint)));

	//dim 1

	std::vector<Geometry::PointPhysical> nodes2D;

	Geometry::PointReference point2D(2),compare2D(2);
	Geometry::PointPhysical point2Dphys(2),compare2Dphys(2);
	Geometry::PointReference orig2D(2);

	jac.resize(2,2);
	jaccompare.resize(2,2);

	for(double i=0.;i<10;++i){
		point2D[0]=1.+i/10.;
		point2D[1]=2.+i/10.;
		nodes2D.push_back(point2D);
	}

	point2D[0]=3.5;
	point2D[1]=4.6;
	nodes2D.push_back(point2D);
	point2D[0]=6.7;
	point2D[1]=2.8;
	nodes2D.push_back(point2D);
	point2D[0]=1.41;
	point2D[1]=6.82;
	nodes2D.push_back(point2D);

	pointIndexes.push_back(12);

	delete left;
	delete test;
	delete right;
	left=new Geometry::ElementGeometry(pointIndexes,nodes2D);
	pointIndexes[2]=10;
	pointIndexes[3]=11;
	right=new Geometry::ElementGeometry(pointIndexes,nodes2D);

	test=new Geometry::FaceGeometry(left,0,right,0);

	leftIndices.resize(2);
	rightIndices.resize(2);

	///\TODO test the normal vector
	//todo also test individual functions

	test->getElementGLeft()->getPhysicalGeometry()->getLocalFaceNodeIndices(test->localFaceNumberLeft(),leftIndices);
	test->getPtrElementGRight()->getPhysicalGeometry()->getLocalFaceNodeIndices(test->localFaceNumberRight(),rightIndices);

	for(int i=0;i<test->getReferenceGeometry()->getNumberOfNodes();++i){
		test->getReferenceGeometry()->getNode(i,orig2D);
		test->getElementGLeft()->getReferenceGeometry()->getNode(leftIndices[i],compare2D);
		test->getElementGLeft()->referenceToPhysical(compare2D,compare2Dphys);
		test->mapRefFaceToRefElemL(orig2D,point2D);
		test->referenceToPhysical(orig2D,point2Dphys);
		assert(("getElementGLeft or localFaceNumberLeft or mapRefFaceToRefElemL",(compare2D[0]-point2D[0])<1e-12));
		assert(("getElementGLeft or localFaceNumberLeft or mapRefFaceToRefElemL",(compare2D[1]-point2D[1])<1e-12));
		assert(("referenceToPhysical",(compare2Dphys[0]-point2Dphys[0])<1e-12));
		assert(("referenceToPhysical",(compare2Dphys[1]-point2Dphys[1])<1e-12));
		test->getPtrElementGRight()->getReferenceGeometry()->getNode(rightIndices[i],compare2D);
		test->getPtrElementGRight()->referenceToPhysical(compare2D,compare2Dphys);
		test->mapRefFaceToRefElemR(orig2D,point2D);
		assert(("getPtrElementGRight or localFaceNumberRight or mapRefFaceToRefElemR or mapRefFaceToRefFace",(compare2D[0]-point2D[0])<1e-12));
		assert(("getPtrElementGRight or localFaceNumberRight or mapRefFaceToRefElemR or mapRefFaceToRefFace",(compare2D[1]-point2D[1])<1e-12));
		assert(("referenceToPhysical",(compare2Dphys[0]-point2Dphys[0])<1e-12));//probably indirectly verified already, but this is the most important feature of a face
		assert(("referenceToPhysical",(compare2Dphys[1]-point2Dphys[1])<1e-12));
	}

	assert((test->getFaceType()==Geometry::INTERNAL));
	assert((typeid(*test->getReferenceGeometry())==typeid(Geometry::ReferenceLine)));



	delete left;
	delete test;
	delete right;
	left=new Geometry::ElementGeometry(pointIndexes,nodes2D);
	pointIndexes[2]=10;
	pointIndexes[3]=11;
	right=new Geometry::ElementGeometry(pointIndexes,nodes2D);

	test=new Geometry::FaceGeometry(left,1,Geometry::WALL_BC);

	leftIndices.resize(2);
	rightIndices.resize(2);

	///\TODO test the normal vector
	//todo also test individual functions

	test->getElementGLeft()->getPhysicalGeometry()->getLocalFaceNodeIndices(test->localFaceNumberLeft(),leftIndices);

	for(int i=0;i<test->getReferenceGeometry()->getNumberOfNodes();++i){
		test->getReferenceGeometry()->getNode(i,orig2D);
		test->getElementGLeft()->getReferenceGeometry()->getNode(leftIndices[i],compare2D);
		test->getElementGLeft()->referenceToPhysical(compare2D,compare2Dphys);
		test->mapRefFaceToRefElemL(orig2D,point2D);
		test->referenceToPhysical(orig2D,point2Dphys);
		assert(("getElementGLeft or localFaceNumberLeft or mapRefFaceToRefElemL",(compare2D[0]-point2D[0])<1e-12));
		assert(("getElementGLeft or localFaceNumberLeft or mapRefFaceToRefElemL",(compare2D[1]-point2D[1])<1e-12));
		assert(("referenceToPhysical",(compare2Dphys[0]-point2Dphys[0])<1e-12));
		assert(("referenceToPhysical",(compare2Dphys[1]-point2Dphys[1])<1e-12));
	}

	assert((test->getFaceType()==Geometry::WALL_BC));
	assert((typeid(*test->getReferenceGeometry())==typeid(Geometry::ReferenceLine)));
	assert((test->getPtrElementGRight()==NULL));
}


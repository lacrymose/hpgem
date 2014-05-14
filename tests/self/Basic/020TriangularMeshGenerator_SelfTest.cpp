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

//This test validates that, for meshes generated by the triangular mesh generator:
//-all IDs assigned to elements, faces or edges are unique (but there may be an edge, a face and an element that share the same ID)
//-all elements connected to a face or an edge agree on the location of this face/edge and on the identity of the face/edge
//-all faces and all edges of an element are defined
///\TODO periodic meshes

#include "Base/MeshManipulator.hpp"
#include "Base/Edge.hpp"
#include "Base/RectangularMeshDescriptor.hpp"

#include "unordered_set"
#include "cassert"

void testMesh(Base::MeshManipulator* test) {
	std::unordered_set<int> elementIDs, faceIDs, edgeIDs;
	for (Base::Element* element : test->getElementsList()) {
		assert(("duplicate element ID", elementIDs.find(element->getID()) == elementIDs.end()));
		elementIDs.insert(element->getID());
		for (int i = 0; i < element->getPhysicalGeometry()->getNrOfFaces();++i) {
			assert(("missing Face",element->getFace(i)!=NULL));
		}
		if(test->dimension()==2){
			assert(("confusion about the number of edges",element->getNrOfEdges()==0));
		}else{
			assert(("confusion about the number of edges",element->getNrOfEdges()==element->getReferenceGeometry()->getNrOfCodim2Entities()));
		}
	}
	for (Base::Face* face : test->getFacesList()) {
		assert(("duplicate face ID", faceIDs.find(face->getID())== faceIDs.end()));
		faceIDs.insert(face->getID());
		assert(("element<->face matching", face->getPtrElementLeft()->getFace(face->localFaceNumberLeft()) == face));
		if (face->isInternal()) {
			std::vector<unsigned int> leftNodes(face->getReferenceGeometry()->getNumberOfNodes()),rightNodes(leftNodes);
			face->getPtrElementLeft()->getPhysicalGeometry()->getGlobalFaceNodeIndices(face->localFaceNumberLeft(), leftNodes);
			face->getPtrElementRight()->getPhysicalGeometry()->getGlobalFaceNodeIndices(face->localFaceNumberRight(), rightNodes);
			for (int i = 0; i < leftNodes.size(); ++i) {
				bool found = false;
				for (int j = 0; j < rightNodes.size(); ++j) {
					found |= leftNodes[i] == rightNodes[j];
				}
				assert(("face positioning", found));
			}
			assert(("face positioning", leftNodes.size() == rightNodes.size()));
			assert(("element<->face matching", face->getPtrElementRight()->getFace(face->localFaceNumberRight()) == face));
		}
	}
	for (Base::Edge* edge : test->getEdgesList()) {
		assert(("duplicate edge ID", edgeIDs.find(edge->getID()) == edgeIDs.end()));
		edgeIDs.insert(edge->getID());
		assert(("element<->edge matching", edge->getElement(0)->getEdge(edge->getEdgeNr(0)) == edge));
		std::vector<unsigned int> firstNodes(edge->getElement(0)->getReferenceGeometry()->getCodim2ReferenceGeometry(edge->getEdgeNr(0))->getNumberOfNodes()),otherNodes(firstNodes);
		edge->getElement(0)->getReferenceGeometry()->getCodim2EntityLocalIndices(edge->getEdgeNr(0), firstNodes);
		for (int i = 0; i < firstNodes.size(); ++i) {
			firstNodes[i] =edge->getElement(0)->getPhysicalGeometry()->getNodeIndex(firstNodes[i]);
		}
		for (int i = 1; i < edge->getNrOfElements(); ++i) {
			assert(("element<->edge matching", edge->getElement(i)->getEdge(edge->getEdgeNr(i)) == edge));
			edge->getElement(i)->getReferenceGeometry()->getCodim2EntityLocalIndices(edge->getEdgeNr(i), otherNodes);
			for (int j = 0; j < otherNodes.size(); ++j) {
				otherNodes[j] =edge->getElement(i)->getPhysicalGeometry()->getNodeIndex(otherNodes[j]);
			}
			for (int k = 0; k < firstNodes.size(); ++k) {
				bool found = false;
				for (int j = 0; j < otherNodes.size(); ++j) {
					found |= firstNodes[k] == otherNodes[j];
				}
				assert(("edge positioning", found));
			}
			assert(("edge positioning", firstNodes.size() == otherNodes.size()));
		}
	}
}

int main(){

	// dim 1
	Base::RectangularMeshDescriptor description1D(1),description2D(2),description3D(3);
	description1D.bottomLeft_[0]=0;
	description2D.bottomLeft_[0]=0;
	description2D.bottomLeft_[1]=0;
	description3D.bottomLeft_[0]=0;
	description3D.bottomLeft_[1]=0;
	description3D.bottomLeft_[2]=0;
	description1D.topRight_[0]=1;
	description2D.topRight_[0]=1;
	description2D.topRight_[1]=1;
	description3D.topRight_[0]=1;
	description3D.topRight_[1]=1;
	description3D.topRight_[2]=1;
	description1D.boundaryConditions_[0]=Base::RectangularMeshDescriptor::SOLID_WALL;
	description2D.boundaryConditions_[0]=Base::RectangularMeshDescriptor::SOLID_WALL;
	description2D.boundaryConditions_[1]=Base::RectangularMeshDescriptor::SOLID_WALL;
	description3D.boundaryConditions_[0]=Base::RectangularMeshDescriptor::SOLID_WALL;
	description3D.boundaryConditions_[1]=Base::RectangularMeshDescriptor::SOLID_WALL;
	description3D.boundaryConditions_[2]=Base::RectangularMeshDescriptor::SOLID_WALL;

	description1D.numElementsInDIM_[0]=2;

	Base::MeshManipulator *test = new Base::MeshManipulator(new Base::ConfigurationData(1,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description1D.bottomLeft_,description1D.topRight_,description1D.numElementsInDIM_);

	testMesh(test);
	assert(("number of elements",test->getNumberOfElements()==2));

	delete test;
	description1D.numElementsInDIM_[0]=3;

	test = new Base::MeshManipulator(new Base::ConfigurationData(1,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description1D.bottomLeft_,description1D.topRight_,description1D.numElementsInDIM_);

	testMesh(test);
	assert(("number of elements",test->getNumberOfElements()==3));

	// dim 2

	delete test;
	description2D.numElementsInDIM_[0]=2;
	description2D.numElementsInDIM_[1]=3;

	test = new Base::MeshManipulator(new Base::ConfigurationData(2,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description2D.bottomLeft_,description2D.topRight_,description2D.numElementsInDIM_);

	testMesh(test);
	assert(("number of elements",test->getNumberOfElements()==12));

	delete test;
	description2D.numElementsInDIM_[0]=3;
	description2D.numElementsInDIM_[1]=2;

	test = new Base::MeshManipulator(new Base::ConfigurationData(2,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description2D.bottomLeft_,description2D.topRight_,description2D.numElementsInDIM_);

	testMesh(test);
	assert(("number of elements",test->getNumberOfElements()==12));

	// dim 3

	delete test;
	description3D.numElementsInDIM_[0]=2;
	description3D.numElementsInDIM_[1]=2;
	description3D.numElementsInDIM_[2]=3;

	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);

	testMesh(test);
	assert(("number of elements",test->getNumberOfElements()==60));

	delete test;
	description3D.numElementsInDIM_[0]=2;
	description3D.numElementsInDIM_[1]=3;
	description3D.numElementsInDIM_[2]=2;

	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);

	testMesh(test);
	assert(("number of elements",test->getNumberOfElements()==60));

	delete test;
	description3D.numElementsInDIM_[0]=3;
	description3D.numElementsInDIM_[1]=2;
	description3D.numElementsInDIM_[2]=2;

	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);

	testMesh(test);
	assert(("number of elements",test->getNumberOfElements()==60));
}




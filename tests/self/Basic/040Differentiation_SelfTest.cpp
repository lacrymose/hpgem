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


//computes \int_\Omega (\nabla f)^2 dx by interpolating f and then integrating using basisFunctionDerivatives

#include "Base/MeshManipulator.hpp"
#include "Base/RectangularMeshDescriptor.hpp"
#include "Integration/ElementIntegral.hpp"
#include "Base/ShortTermStorageElementH1.hpp"
#include "Integration/ElementIntegrandBase.hpp"

#include "unordered_set"
#include "cassert"

#include "Utilities/BasisFunctions1DH1ConformingLine.hpp"
#include "Utilities/BasisFunctions2DH1ConformingSquare.hpp"
#include "Utilities/BasisFunctions2DH1ConformingTriangle.hpp"
#include "Utilities/BasisFunctions3DH1ConformingCube.hpp"
#include "Utilities/BasisFunctions3DH1ConformingTetrahedron.hpp"
#include "Base/ConfigurationData.hpp"
#include "Base/L2Norm.hpp"
#include "Base/ElementCacheData.hpp"

void testMesh(Base::MeshManipulator* test) {
	class :public Integration::ElementIntegrandBase<LinearAlgebra::Matrix>{
		void elementIntegrand(const Base::Element* el, const Geometry::PointReference& p, LinearAlgebra::Matrix& ret){
			int n=el->getNrOfBasisFunctions();
			Geometry::PointPhysical pPhys(p.size());
			el->referenceToPhysical(p,pPhys);
			ret.resize(1,n);
			for(int i=0;i<n;++i){
				ret[i]=el->basisFunction(i,p);
				for(int j=0;j<p.size();++j){
					ret[i]*=pPhys[j];
				}
			}
		}
	}interpolation;
	class :public Integration::ElementIntegrandBase<LinearAlgebra::Matrix>{
		void elementIntegrand(const Base::Element* el, const Geometry::PointReference& p, LinearAlgebra::Matrix& ret){
			int n=el->getNrOfBasisFunctions();
			ret.resize(n,n);
			for(int i=0;i<n;++i){
				for(int j=0;j<n;++j){
					ret(i,j)=el->basisFunction(i,p)*el->basisFunction(j,p);
				}
			}
		}
	}massMatrix;
	class a:public Integration::ElementIntegrandBase<LinearAlgebra::NumericalVector>{
		void elementIntegrand(const Base::Element* el, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret){
			ret.resize(1);
			int n=el->getNrOfBasisFunctions();
			LinearAlgebra::NumericalVector temp1(p.size()),temp2(p.size());
			for(int i=0;i<p.size();++i){
				temp1[i]=0;
			}
			for(int i=0;i<n;++i){
				temp2.resize(p.size());
				el->basisFunctionDeriv(i,p,temp2);
				temp1+=temp2*el->getData(0,0,i);
                                //std::cout<<temp2<<" "<<el->getData(0,0,i)<<std::endl;
			}
			ret[0]=Base::L2Norm(temp1)*Base::L2Norm(temp1);
                        //std::cout<<std::endl;
                        //std::cout<<ret[0]<<std::endl;
		}
	}integrating;
	std::cout.precision(14);
	Integration::ElementIntegral elIntegral(false);
	elIntegral.setStorageWrapper(new Base::ShortTermStorageElementH1(test->dimension()));
	double total=0;
	LinearAlgebra::NumericalVector result(1);
	LinearAlgebra::Matrix expansion, M;
	for(Base::Element* element:test->getElementsList()){
		elIntegral.integrate(element,&interpolation,expansion);
		elIntegral.integrate(element,&massMatrix,M);
		M.inverse(M);
		expansion=expansion*M;
		element->setTimeLevelData(0,expansion);
		elIntegral.integrate(element,&integrating,result);
                //std::cout<<result[0]<<std::endl;
		total+=result[0];
	}

	std::cout<<total<<" "<<std::endl;
	//assert(("derivatives",fabs(total-4./3.+1./3.*test->dimension())<1e-12));
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
          //1D triangular meshes dont exist
	Base::MeshManipulator *test = new Base::MeshManipulator(new Base::ConfigurationData(1,1,2,1),false,false,false,2,0);
	test->createRectangularMesh(description1D.bottomLeft_,description1D.topRight_,description1D.numElementsInDIM_);
	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet1DH1Line(2));
	testMesh(test);

	delete test;
	description1D.numElementsInDIM_[0]=3;
	test = new Base::MeshManipulator(new Base::ConfigurationData(1,1,2,1),false,false,false,2,0);
	test->createRectangularMesh(description1D.bottomLeft_,description1D.topRight_,description1D.numElementsInDIM_);
	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet1DH1Line(2));
	testMesh(test);

	// dim 2

	delete test;
	description2D.numElementsInDIM_[0]=2;
	description2D.numElementsInDIM_[1]=3;

	test = new Base::MeshManipulator(new Base::ConfigurationData(2,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description2D.bottomLeft_,description2D.topRight_,description2D.numElementsInDIM_);

	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet2DH1Triangle(2));
	testMesh(test);

	delete test;
	test = new Base::MeshManipulator(new Base::ConfigurationData(2,1,2,1),false,false,false,2,0);
	test->createRectangularMesh(description2D.bottomLeft_,description2D.topRight_,description2D.numElementsInDIM_);
	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet2DH1Square(2));
	testMesh(test);

	delete test;
	description2D.numElementsInDIM_[0]=3;
	description2D.numElementsInDIM_[1]=2;

	test = new Base::MeshManipulator(new Base::ConfigurationData(2,1,2,1),false,false,false,2,0);
	test->createTriangularMesh(description2D.bottomLeft_,description2D.topRight_,description2D.numElementsInDIM_);

	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet2DH1Triangle(2));
	testMesh(test);

	delete test;
	test = new Base::MeshManipulator(new Base::ConfigurationData(2,1,2,1),false,false,false,2,0);
	test->createRectangularMesh(description2D.bottomLeft_,description2D.topRight_,description2D.numElementsInDIM_);
	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet2DH1Square(2));
	testMesh(test);
	// dim 3

	delete test;
	description3D.numElementsInDIM_[0]=2;
	description3D.numElementsInDIM_[1]=2;
	description3D.numElementsInDIM_[2]=3;

	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,3,1),false,false,false,3,0);
	test->createTriangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);

	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet3DH1Tetrahedron(3));
	testMesh(test);

	delete test;
	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,2,1),false,false,false,2,0);
	test->createRectangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);
	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet3DH1Cube(2));
	testMesh(test);

	delete test;
	description3D.numElementsInDIM_[0]=2;
	description3D.numElementsInDIM_[1]=3;
	description3D.numElementsInDIM_[2]=2;

	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,3,1),false,false,false,3,0);
	test->createTriangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);

	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet3DH1Tetrahedron(3));
	testMesh(test);

	delete test;
	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,2,1),false,false,false,2,0);
	test->createRectangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);
	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet3DH1Cube(2));
	testMesh(test);

	delete test;
	description3D.numElementsInDIM_[0]=3;
	description3D.numElementsInDIM_[1]=2;
	description3D.numElementsInDIM_[2]=2;

	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,3,1),false,false,false,3,0);
	test->createTriangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);

	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet3DH1Tetrahedron(3));
	testMesh(test);

	delete test;
	test = new Base::MeshManipulator(new Base::ConfigurationData(3,1,2,1),false,false,false,2,0);
	test->createRectangularMesh(description3D.bottomLeft_,description3D.topRight_,description3D.numElementsInDIM_);
	testMesh(test);
	test->setDefaultBasisFunctionSet(Utilities::createDGBasisFunctionSet3DH1Cube(2));
	testMesh(test);
}


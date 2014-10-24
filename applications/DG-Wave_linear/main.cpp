/*
 * main.cpp
 *
 *  Created on: Feb 28, 2014
 *      Author: brinkf
 */

#define hpGEM_INCLUDE_PETSC_SUPPORT

#include "Base/HpgemUI.hpp"
#include "Base/Norm2.hpp"
#include "Output/TecplotSingleElementWriter.hpp"
#include <fstream>
#include "Utilities/BasisFunctions2DH1ConformingSquare.hpp"
#include "Integration/ElementIntegrandBase.hpp"
#include "Integration/FaceIntegrandBase.hpp"
#include "Integration/FaceIntegral.hpp"
#include "Integration/ElementIntegral.hpp"
#include "Base/GlobalData.hpp"
#include "Base/RectangularMeshDescriptor.hpp"
#include "Base/ConfigurationData.hpp"
#include "Base/Element.hpp"
#include "Base/Face.hpp"
#include "Output/TecplotDiscontinuousSolutionWriter.hpp"
#include "Base/ElementCacheData.hpp"
#include "Base/FaceCacheData.hpp"
#include <cmath>

#include "Utilities/GlobalMatrix.hpp"
#include "Utilities/GlobalVector.hpp"
#include "petscksp.h"

const unsigned int DIM=2;

class DGWave : public Base::HpgemUI,public Output::TecplotSingleElementWriter{

public:	
	DGWave(int n,int p):HpgemUI(new Base::GlobalData(),new Base::ConfigurationData(DIM,2,p)),n_(n),p_(p){}

	///set up the mesh
    bool virtual initialise(){
    	RectangularMeshDescriptorT description(DIM);
    	for(int i=0;i<DIM;++i){
    		description.bottomLeft_[i]=0;
    		description.topRight_[i]=1;
    		description.numElementsInDIM_[i]=n_;
    		description.boundaryConditions_[i]=RectangularMeshDescriptorT::PERIODIC;
    	}
        description.topRight_[DIM-1]=-1;
        description.numElementsInDIM_[DIM-1]/=8;
        description.boundaryConditions_[DIM-1]=RectangularMeshDescriptorT::SOLID_WALL;
    	addMesh(description,Base::RECTANGULAR,1,1,1,1);
    	meshes_[0]->setDefaultBasisFunctionSet(Utilities::createInteriorBasisFunctionSet2DH1Square(p_));
    	std::vector<const Base::BasisFunctionSet*> bFsets;
    	Utilities::createVertexBasisFunctionSet2DH1Square(p_,bFsets);
    	meshes_[0]->addVertexBasisFunctionSet(bFsets);
    	std::vector<const Base::OrientedBasisFunctionSet*> oBFsets;
    	Utilities::createFaceBasisFunctionSet2DH1Square(p_,oBFsets);
    	meshes_[0]->addFaceBasisFunctionSet(oBFsets);
    	return true;
    }

    class :public Integration::ElementIntegrandBase<LinearAlgebra::Matrix>{
    public:
        void elementIntegrand(const Base::Element* element, const Geometry::PointReference& p, LinearAlgebra::Matrix& ret){
            int n=element->getNrOfBasisFunctions();
            ret.resize(n,n);
            LinearAlgebra::NumericalVector phiDerivI(DIM),phiDerivJ(DIM);
            for(int i=0;i<n;++i){
                element->basisFunctionDeriv(i,p,phiDerivI);
                for(int j=0;j<=i;++j){
                    element->basisFunctionDeriv(j,p,phiDerivJ);
                    ret(i,j)=ret(j,i)=phiDerivI*phiDerivJ;
                }
            }
        }
    }stifnessIntegrand;
    
    class :public Integration::FaceIntegrandBase<LinearAlgebra::Matrix>{
    public:
        void faceIntegrand(const Base::Face* face, const LinearAlgebra::NumericalVector& normal, const Geometry::PointReference& p, LinearAlgebra::Matrix& ret){
            int n=face->getNrOfBasisFunctions();
            ret.resize(n,n);
            for(int i=0;i<n;++i){
                for(int j=0;j<n;++j){//the basis functions belonging to internal parameters are 0 on the free surface anyway.
                    ret(i,j)=face->basisFunction(i,p)*face->basisFunction(j,p);
                }
            }
        }
    }massIntegrand;

    static void initialConditions(const PointPhysicalT& p,LinearAlgebra::NumericalVector& ret){
        ret.resize(2);
    	//ret[0]=cos(2*M_PI*p[0])*cosh(2*M_PI*(p[DIM-1]+1))/cosh(2*M_PI);//standing wave
        //ret[1]=0;
        double k=2.*M_PI;
        ret[0]=cosh(k*(p[DIM-1]+1))*sin(-k*p[0])*sqrt(k*tanh(k))/cosh(k)*0.001;//moving wave
        ret[1]=cosh(k*(p[DIM-1]+1))*cos(-k*p[0])/cosh(k)*0.001;
    }
    
    static void exactSolution(const double t, const PointPhysicalT& p, LinearAlgebra::NumericalVector& ret){
        ret.resize(2);
        //ret[0]=cos(2*M_PI*p[0])*cos(sqrt(2*M_PI*tanh(2*M_PI))*t)*cosh(2*M_PI*(p[DIM-1]+1))/cosh(2*M_PI);//standing wave
        //ret[1]=cos(2*M_PI*p[0])*sin(-sqrt(2*M_PI*tanh(2*M_PI))*t)/sqrt(2*M_PI*tanh(2*M_PI))*cosh(2*M_PI*(p[DIM-1]+1))/cosh(2*M_PI);
        double k=2.*M_PI;
        ret[0]=cosh(k*(p[DIM-1]+1))*sin(sqrt(k*tanh(k))*t-k*p[0])*sqrt(k*tanh(k))/cosh(k)*0.001;//moving wave
        ret[1]=cosh(k*(p[DIM-1]+1))*cos(sqrt(k*tanh(k))*t-k*p[0])/cosh(k)*0.001;
        
    }

    class :public Integration::FaceIntegrandBase<LinearAlgebra::Matrix>{
    public:
        void faceIntegrand(const FaceT* element, const LinearAlgebra::NumericalVector&, const PointReferenceT& p, LinearAlgebra::Matrix& ret){
            PointPhysicalT pPhys(DIM);
            element->referenceToPhysical(p,pPhys);
            ret.resize(2,element->getNrOfBasisFunctions());
            LinearAlgebra::NumericalVector data;
            initialConditions(pPhys,data);
            for(int i=0;i<element->getNrOfBasisFunctions();++i){
                    ret(0,i)=element->basisFunction(i,p)*data[0];
                    ret(1,i)=element->basisFunction(i,p)*data[1];
            }
        }
    }interpolator;
    
    class :public Integration::FaceIntegrandBase<LinearAlgebra::NumericalVector>{
    public:
        virtual void faceIntegrand(const FaceT* face,const LinearAlgebra::NumericalVector& normal, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret){
            PointPhysicalT pPhys(DIM);
            PointReferenceT pElement(DIM);
            face->referenceToPhysical(p,pPhys);
            face->mapRefFaceToRefElemL(p,pElement);
            ret.resize(2);
            static LinearAlgebra::NumericalVector exact(2);
            if(fabs(pPhys[DIM-1])<1e-9){
                face->getPtrElementLeft()->getSolution(0,pElement,ret);
                exactSolution(t,pPhys,exact);
                ret-=exact;
                ret[0]*=ret[0];
                ret[1]*=ret[1];
            }
        }
    }error;
    
    class :public Integration::FaceIntegrandBase<LinearAlgebra::NumericalVector>{
        public:
        virtual void faceIntegrand(const FaceT* face,const LinearAlgebra::NumericalVector& normal, const PointReferenceT& p, LinearAlgebra::NumericalVector& ret){
            PointPhysicalT pPhys(DIM);
            PointReferenceT pElement(DIM);
            face->referenceToPhysical(p,pPhys);
            face->mapRefFaceToRefElemL(p,pElement);
            ret.resize(1);
            static LinearAlgebra::NumericalVector dummySolution(2),gradPhi(DIM),temp(DIM);
            const LinearAlgebra::Matrix& expansioncoefficients=face->getPtrElementLeft()->getTimeLevelData(0);
            if(fabs(pPhys[DIM-1])<1e-9){
                dummySolution[0]=0;
                for(int i=0;i<face->getNrOfBasisFunctions();++i){
                    dummySolution[0]+=face->basisFunction(i,p)*expansioncoefficients(0,i);
                }
                ret[0]=dummySolution[0]*dummySolution[0];
                ret[0]/=2;//assumes g=1
            }
        }
    }faceEnergy;

    class :public Integration::ElementIntegrandBase<LinearAlgebra::NumericalVector>{
    public:
        void elementIntegrand(const Base::Element* element, const Geometry::PointReference& p, LinearAlgebra::NumericalVector& ret){
            int n=element->getNrOfBasisFunctions();
            ret.resize(1);
            LinearAlgebra::NumericalVector gradPhi(DIM),temp(DIM);
            const LinearAlgebra::Matrix& expansioncoefficients=element->getTimeLevelData(0);
            for(int i=0;i<n;++i){
                element->basisFunctionDeriv(i,p,temp);
                temp*=expansioncoefficients(1,i);
                gradPhi+=temp;
            }
            ret[0]=(gradPhi*gradPhi)/2;
        }
    }elementEnergy;

    void writeToTecplotFile(const ElementT* element,const  PointReferenceT& p, std::ostream& out){
    	LinearAlgebra::NumericalVector value(2);
    	element->getSolution(0,p,value);
    	out<< value[0] << " " << value[1];
        Geometry::PointPhysical pPhys(DIM);
        element->referenceToPhysical(p,pPhys);
        exactSolution(t,pPhys,value);
    	out << " " << value[0] << " " << value[1];
    }
    
    //assumes that 'all' means 'all relevant'; computes the mass matrix at the free surface
    void doAllFaceIntegration(){
        Integration::FaceIntegral integral(false);
        Geometry::PointReference p(DIM-1);
        Geometry::PointPhysical pPhys(DIM);
        LinearAlgebra::Matrix result,initialconditions;
        for(Base::Face* face:meshes_[0]->getFacesList()){
            face->getReferenceGeometry()->getCenter(p);
            face->referenceToPhysical(p,pPhys);
            if(fabs(pPhys[DIM-1])<1e-9){
                integral.integrate(face,&massIntegrand,result);
                integral.integrate(face,&interpolator,initialconditions);
                face->getPtrElementLeft()->setTimeLevelData(0,initialconditions);
            }else{
                result.resize(face->getNrOfBasisFunctions(),face->getNrOfBasisFunctions());
                for(int i=0;i<result.size();++i){
                    result[i]=0;
                }
            }
            face->setFaceMatrix(result);
        }
    }
    
    void doAllElementIntegration(){
        Integration::ElementIntegral integral(false);
        LinearAlgebra::Matrix result;
        for(Base::Element* element:meshes_[0]->getElementsList()){
            integral.integrate(element,&stifnessIntegrand,result);
            element->setElementMatrix(result);
        }
    }

    //messy routine that collects the row numbers of basisfunctions active on the free surface for use with PETSc
    void getSurfaceIS(Utilities::GlobalPetscMatrix& S, IS* surface, IS* rest){
        Geometry::PointReference p(DIM);
        Geometry::PointPhysical pPhys(DIM);
        int n(0);
        std::vector<int> facePositions;
        for(const Base::Face* face:meshes_[0]->getFacesList()){
            face->getReferenceGeometry()->getCenter(p);
            face->referenceToPhysical(p,pPhys);
            if(fabs(pPhys[DIM-1])<1e-9){
                S.getMatrixBCEntries(face,n,facePositions);
            }
        }
        ISCreateGeneral(MPI_COMM_WORLD,n,&facePositions[0],PETSC_COPY_VALUES,surface);
        MatGetSize(S,&n,NULL);
        ISSort(*surface);
        ISComplement(*surface,0,n,rest);
        ISDestroy(surface);
        ISComplement(*rest,0,n,surface);//the complement of the complement does not contain duplicates
    }
    
    void printError(){
        Integration::FaceIntegral integral(false);
        LinearAlgebra::NumericalVector totalError(2),contribution(2);
        for(Base::Face* face:meshes_[0]->getFacesList()){
            integral.integrate(face,&error,contribution);
            totalError+=contribution;
            contribution[0]=0;
            contribution[1]=0;
        }
        std::cout<<"t: "<<t<<" eta: "<<sqrt(totalError[0])<<" phi: "<<sqrt(totalError[1])<<std::endl;
    }
    
    void computeEnergy(){
        Integration::ElementIntegral elIntegral(false);
        Integration::FaceIntegral faIntegral(false);
        LinearAlgebra::NumericalVector totalEnergy(1),contribution(1);
        for(Base::Face* face:meshes_[0]->getFacesList()){
            faIntegral.integrate(face,&faceEnergy,contribution);
            totalEnergy+=contribution;
            contribution[0]=0;
        }
        for(Base::Element* element:meshes_[0]->getElementsList()){
            elIntegral.integrate(element,&elementEnergy,contribution);
            totalEnergy+=contribution;
            contribution[0]=0;
        }
        std::cout<<"Energy: "<<totalEnergy[0]<<std::endl;
    }
    
    bool solve(){
        std::cout.precision(14);
        doAllElementIntegration();
        doAllFaceIntegration();
        
        Utilities::GlobalPetscVector eta(meshes_[0]),phi(meshes_[0]);
        Utilities::GlobalPetscMatrix M(meshes_[0],-1,0),S(meshes_[0],0,-1);
        Vec phiS,phiOther,etaActually,interiorRHS,surfaceRHS;
        Mat surfaceMass,interiorStifness,surfaceStifness,mixStifness,backStiffness;
        
        IS isSurface,isRest;
        getSurfaceIS(S,&isSurface,&isRest);
        
    	std::ofstream outFile("output.dat");
        Output::TecplotDiscontinuousSolutionWriter writeFunc(outFile,"test","01","eta_num,phi_num,eta_exact,phi_exact");
        
    	//deal with initial conditions
        double g(1.),dt(M_PI/n_/2);
        eta.constructFromTimeLevelData(0,0);
        phi.constructFromTimeLevelData(0,1);
        
    	int numberOfSnapshots(65);//placeholder parameters
    	int numberOfTimeSteps(n_/8);
        
        MatGetSubMatrix(M,isSurface,isSurface,MAT_INITIAL_MATRIX,&surfaceMass);
        MatGetSubMatrix(S,isRest,isRest,MAT_INITIAL_MATRIX,&interiorStifness);
        MatGetSubMatrix(S,isSurface,isSurface,MAT_INITIAL_MATRIX,&surfaceStifness);
        MatGetSubMatrix(S,isRest,isSurface,MAT_INITIAL_MATRIX,&mixStifness);
        MatGetSubMatrix(S,isSurface,isRest,MAT_INITIAL_MATRIX,&backStiffness);
        
        KSP interior,surface;
    	KSPCreate(MPI_COMM_WORLD,&interior);
    	KSPSetOperators(interior,interiorStifness,interiorStifness);
        KSPSetInitialGuessNonzero(interior,PETSC_TRUE);
    	KSPSetFromOptions(interior);
        KSPCreate(MPI_COMM_WORLD,&surface);
        KSPSetOperators(surface,surfaceMass,surfaceMass);
        KSPSetFromOptions(surface);
        KSPConvergedReason conferge;
        int iterations;
        
        
        VecGetSubVector(phi,isSurface,&phiS);
        VecGetSubVector(phi,isRest,&phiOther);
        VecGetSubVector(eta,isSurface,&etaActually);
        VecDuplicate(phiS,&surfaceRHS);
        VecDuplicate(phiOther,&interiorRHS);
        VecCopy(phiS,surfaceRHS);
        KSPSolve(surface,surfaceRHS,phiS);
        KSPGetConvergedReason(surface,&conferge);
        KSPGetIterationNumber(surface,&iterations);
        std::cout<<"Finalizing interpolation (1): KSP solver ended because of "<<KSPConvergedReasons[conferge]<<" in "<<iterations<<" iterations."<<std::endl;
        VecCopy(etaActually,surfaceRHS);
        KSPSolve(surface,surfaceRHS,etaActually);
        KSPGetConvergedReason(surface,&conferge);
        KSPGetIterationNumber(surface,&iterations);
        std::cout<<"Finalizing interpolation (2): KSP solver ended because of "<<KSPConvergedReasons[conferge]<<" in "<<iterations<<" iterations."<<std::endl;
        VecRestoreSubVector(eta,isSurface,&etaActually);
        VecRestoreSubVector(phi,isSurface,&phiS);
        VecRestoreSubVector(phi,isRest,&phiOther);
        
        eta.writeTimeLevelData(0,0);
        phi.writeTimeLevelData(0,1);
        
        writeFunc.write(meshes_[0],"solution at time 0",false,this);
    	for(int i=1;i<numberOfSnapshots;++i){
            VecGetSubVector(phi,isSurface,&phiS);
            VecGetSubVector(phi,isRest,&phiOther);
            VecGetSubVector(eta,isSurface,&etaActually);
            for(int j=0;j<numberOfTimeSteps;++j){
                t+=dt;
                VecAXPY(phiS,-g*dt/2,etaActually);      //(can in principle be combined with final step, but this makes output easier)
                MatMult(mixStifness,phiS,interiorRHS);
                VecScale(interiorRHS,-1);
                KSPSolve(interior,interiorRHS,phiOther);
                KSPGetConvergedReason(interior,&conferge);
                KSPGetIterationNumber(interior,&iterations);
                std::cout<<"Laplace problem: KSP solver ended because of "<<KSPConvergedReasons[conferge]<<" in "<<iterations<<" iterations."<<std::endl;
                
                MatMult(backStiffness,phiOther,surfaceRHS);
                MatMultAdd(surfaceStifness,phiS,surfaceRHS,surfaceRHS);
                VecScale(surfaceRHS,dt);
                MatMultAdd(surfaceMass,etaActually,surfaceRHS,surfaceRHS);
                KSPSolve(surface,surfaceRHS,etaActually);
                KSPGetConvergedReason(surface,&conferge);
                KSPGetIterationNumber(surface,&iterations);
                std::cout<<"Updating \\eta: KSP solver ended because of "<<KSPConvergedReasons[conferge]<<" in "<<iterations<<" iterations."<<std::endl;
                
                VecAXPY(phiS,-g*dt/2,etaActually);      
            }
            VecRestoreSubVector(phi,isSurface,&phiS);
            VecRestoreSubVector(phi,isRest,&phiOther);
            VecRestoreSubVector(eta,isSurface,&etaActually);
            eta.writeTimeLevelData(0,0);
            phi.writeTimeLevelData(0,1);
            writeFunc.write(meshes_[0],"solution at time t",false,this);
            printError();
            computeEnergy();
    	}
        return true;
    }

private:

    //number of elements per cardinal direction
    int n_;

    //polynomial order of the approximation
    int p_;
    static double t;
};

double DGWave::t=0;

int main(int argc, char **argv){
	try{
		int n,p;
		if(argc>2){//convert the first two arguments to numbers
			n=std::atoi(argv[1]);
			p=std::atoi(argv[2]);
			argv[2]=argv[0];//and remove them from the argument list
			argc-=2;
			argv+=2;
		}else{
			std::cout<<"usage: LinearPotentialFlow.out n p [petsc-args]\nDefaulting to n=64;p=1;"<<std::endl;
                        n=64;
                        p=1;
		}
		PetscInitialize(&argc,&argv,NULL,NULL);
		DGWave test(n,p);
		test.initialise();
		test.solve();
		PetscFinalize();
		return 0;
	}
	catch(const char* e){
		std::cout<<e;
	}
}






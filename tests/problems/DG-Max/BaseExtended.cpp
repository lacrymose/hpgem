#include "BaseExtended.hpp"
#include "kspaceData.hpp"

template<>
void hpGemUIExtentions<3>::setConfigData() {
    const_cast<Base::ConfigurationData*>(configData_)->numberOfBasisFunctions_=globalData_->numberOfUnknowns_;
    const_cast<Base::GlobalData*>(globalData_)->numberOfTimeLevels_=configData_->numberOfTimeLevels_;
    const_cast<Base::ConfigurationData*>(configData_)->numberOfUnknowns_=1;
}

template<>
const Base::ConfigurationData* hpGemUIExtentions<3>::getConfigData() {
    return configData_;
}

template<>
const MaxwellData* hpGemUIExtentions<3>::getData() const {
    return dynamic_cast<const MaxwellData*>(globalData_);
}

template<>
void hpGemUIExtentions<3>::initialConditions(const Base::HpgemUI< 3 >::ElementT* element, const PointElementReferenceT& p, LinearAlgebra::Matrix& ret) {
    ret.resize(element->getNrOfBasisFunctions(),1);
    ElementInfos* info = static_cast<ElementInfos*>(const_cast<ElementT*>(element)->getUserData());
    PointPhysicalT pPhys;
    element->referenceToPhysical(p,pPhys);
    NumericalVector val(3),phi(3);
    std::vector<NumericalVector> functionValues;
    info->makeFunctionValuesVector(element,p,functionValues);
    initialConditions(pPhys,val);
    for(int i=0; i<element->getNrOfBasisFunctions(); ++i) {
	phi=functionValues[i];
	ret(i,0)=phi[0]*val[0]+phi[1]*val[1]+phi[2]*val[2];
    }
}

template<>
void hpGemUIExtentions<3>::sourceTerm(const Base::HpgemUI< 3 >::ElementT* element, const PointElementReferenceT& p, LinearAlgebra::Matrix& ret) {
    ret.resize(element->getNrOfBasisFunctions(),1);
    ElementInfos* info = static_cast<ElementInfos*>(const_cast<ElementT*>(element)->getUserData());
    PointPhysicalT pPhys;
    element->referenceToPhysical(p,pPhys);
    NumericalVector val(3),phi(3);
    std::vector<NumericalVector> functionValues;
    info->makeFunctionValuesVector(element,p,functionValues);
    sourceTerm(pPhys,val);
    for(int i=0; i<element->getNrOfBasisFunctions(); ++i) {
	phi=functionValues[i];
	ret(i,0)=phi[0]*val[0]+phi[1]*val[1]+phi[2]*val[2];
    }
}

template<>
void hpGemUIExtentions<3>::sourceTermBoundaryIP(const Base::HpgemUI< 3 >::FaceT* face, const Base::HpgemUI< 3 >::PointPhysicalT& normal, const PointFaceReferenceT& p, Matrix& ret){
    ElementT* left=const_cast<ElementT*>(face->getPtrElementLeft());
    ElementInfos* info = static_cast<ElementInfos*>(left->getUserData());
    ret.resize(left->getNrOfBasisFunctions(),1);
    PointReferenceT PLeft;
    face->mapRefFaceToRefElemL(p,PLeft);
    std::vector<NumericalVector> functionValues,functionCurls;
    info->makeFunctionValuesVector(left,PLeft,functionValues);
    info->makeFunctionCurlsVector(left,PLeft,functionCurls);
    PointPhysicalT PPhys;
    left->referenceToPhysical(PLeft,PPhys);
    NumericalVector normedNormal(3);
    normedNormal[0] = (normal*(1/Base::L2Norm<3>(normal)))[0];
    normedNormal[1] = (normal*(1/Base::L2Norm<3>(normal)))[1];
    normedNormal[2] = (normal*(1/Base::L2Norm<3>(normal)))[2];
    NumericalVector val(3),phi(3),phi_curl(3),dummy(3);
    initialConditions(PPhys,dummy);//assumes the initial conditions and the boundary conditions match
    OuterProduct(normedNormal,dummy,val);
    for(int i=0; i<left->getNrOfBasisFunctions(); ++i) {
	dummy=functionValues[i];
	phi_curl=functionCurls[i];
	OuterProduct(normedNormal,dummy,phi);
	ret(i,0)=-(phi_curl[0]*val[0]+phi_curl[1]*val[1]+phi_curl[2]*val[2])+
		  getData()->StabCoeff_*(phi[0]*val[0]+phi[1]*val[1]+phi[2]*val[2]);
    }
}

template<>
void hpGemUIExtentions<3>::sourceTermBoundary(const Base::HpgemUI< 3 >::FaceT* face, const Base::HpgemUI< 3 >::PointPhysicalT& normal, const PointFaceReferenceT& p, Matrix& ret) {
    ElementT* left=const_cast<ElementT*>(face->getPtrElementLeft());
    ElementInfos* info = static_cast<ElementInfos*>(left->getUserData());
    ret.resize(left->getNrOfBasisFunctions(),1);
    PointElementReferenceT PLeft;
    face->mapRefFaceToRefElemL(p,PLeft);
    std::vector<NumericalVector> functionCurls;
    info->makeFunctionCurlsVector(left,PLeft,functionCurls);
    PointPhysicalT PPhys;
    left->referenceToPhysical(PLeft,PPhys);
    NumericalVector normedNormal(3);
    normedNormal[0] = (normal*(1/Base::L2Norm<3>(normal)))[0];
    normedNormal[1] = (normal*(1/Base::L2Norm<3>(normal)))[1];
    normedNormal[2] = (normal*(1/Base::L2Norm<3>(normal)))[2];
    NumericalVector val(3),dummy(3),phi_curl(3);
    initialConditions(PPhys,dummy);//assumes the initial conditions and the boundary conditions match
    OuterProduct(normedNormal,dummy,val);
    for(int i=0; i<left->getNrOfBasisFunctions(); ++i) {
	phi_curl=functionCurls[i];
	ret(i,0)=-(phi_curl[0]*val[0]+phi_curl[1]*val[1]+phi_curl[2]*val[2]);
    }
}

template<>
void hpGemUIExtentions<3>::sourceTermBoundaryBR(const Base::HpgemUI< 3 >::FaceT* face, const Base::HpgemUI< 3 >::PointPhysicalT& normal, const PointFaceReferenceT& p, Matrix& ret) {
    ElementT* left=const_cast<ElementT*>(face->getPtrElementLeft());
    ElementInfos* info = static_cast<ElementInfos*>(left->getUserData());
    ret.resize(left->getNrOfBasisFunctions(),1);
    PointElementReferenceT PLeft;
    face->mapRefFaceToRefElemL(p,PLeft);
    std::vector<NumericalVector> functionValues;
    info->makeFunctionValuesVector(left,PLeft,functionValues);
    PointPhysicalT PPhys;
    left->referenceToPhysical(PLeft,PPhys);
    NumericalVector normedNormal(3);
    normedNormal[0] = (normal*(1/Base::L2Norm<3>(normal)))[0];
    normedNormal[1] = (normal*(1/Base::L2Norm<3>(normal)))[1];
    normedNormal[2] = (normal*(1/Base::L2Norm<3>(normal)))[2];
    NumericalVector val(3),dummy(3),phi(3);
    initialConditions(PPhys,dummy);
    OuterProduct(normedNormal,dummy,val);
    for(int i=0; i<left->getNrOfBasisFunctions(); ++i) {
	phi=functionValues[i];
	ret(i,0)=2*(phi[0]*val[0]+phi[1]*val[1]+phi[2]*val[2]);
    }
}

template<>
void hpGemUIExtentions<3>::initialConditionsDeriv(const Base::HpgemUI< 3 >::ElementT* element, const PointElementReferenceT& p, LinearAlgebra::Matrix& ret){
    ret.resize(element->getNrOfBasisFunctions(),1);
    ElementInfos* info = static_cast<ElementInfos*>(const_cast<ElementT*>(element)->getUserData());
    PointPhysicalT pPhys;
    element->referenceToPhysical(p,pPhys);
    std::vector<NumericalVector> functionValues;
    info->makeFunctionValuesVector(element,p,functionValues);
    NumericalVector val(3),phi(3);
    initialConditionsDeriv(pPhys,val);
    for(int i=0; i<element->getNrOfBasisFunctions(); ++i) {
	phi=functionValues[i];
	ret(i,0)=phi[0]*val[0]+phi[1]*val[1]+phi[2]*val[2];
    }
}

template<>
void hpGemUIExtentions<3>::writeFieldValues(const Base::HpgemUI< 3 >::ElementT& element, const PointElementReferenceT& p, ostream& output){
    ElementInfos* info = static_cast<ElementInfos*>(const_cast<ElementT*>(&element)->getUserData());
    LinearAlgebra::Matrix data=const_cast<ElementT*>(&element)->getTimeLevelData(timelevel_);
    NumericalVector results(3),curls(3),waveDirection(3),Eorth(3),Horth(3);
    vector<NumericalVector> values,curlsVec;
    info->makeFunctionValuesVector(&element,p,values);
    info->makeFunctionCurlsVector(&element,p,curlsVec);
    for(int i=0; i<element.getNrOfBasisFunctions(); ++i) {
	results+=values[i]*data[i];
	curls+=curlsVec[i]*data[i];
    }
    output<<results[0]<<" "<<results[1]<<" "<<results[2]<<" "<<curls[0]<<" "<<curls[1]<<" "<<curls[2]<<endl;
}
/*
template<>
void hpGemUIExtentions<3>::writeTecplotFile(const Base::HpgemUI< 3 >::MeshManipulatorT& mesh, const char* zonetitle, const int timelevel, std::ofstream& file, const bool existingFile){
    int numberOfNodes;
    int numberOfElements;//tecplot somewhy messes up the ordering of the elements for linked zones so dont reuse this
    long int numberOfNodesPosition;
    long int numberOfElementsPosition;
    if(!existingFile) {
	file<<"TITLE = \"The electric field\"\nVARIABLES = \"x0\", \"x1\", \"x2\", \"E0\", \"E1\", \"E2\", \"H0\", \"H1\", \"H2\"\n";
    }
    file<<"ZONE T = \""<<zonetitle<<"\", ZONETYPE = FETETRAHEDRON, DATAPACKING = POINT, N = ";
    numberOfNodesPosition = file.tellp();
    file<<"         , E = ";
    numberOfElementsPosition = file.tellp();
    file<<"        \n";
    numberOfElements=0;
    numberOfNodes=0;
    PointElementReferenceT p;
    PointPhysicalT pPhys;
    for(ConstElementIterator it=mesh.elementColBegin(); it!=mesh.elementColEnd(); ++it) {
	++numberOfElements;
	numberOfNodes+=4;
	for(int i=0; i<4; ++i) {
	    (*it)->getReferenceGeometry()->getNode(i,p);
	    (*it)->referenceToPhysical(p,pPhys);
	    file<<pPhys[0]<<" "<<pPhys[1]<<" "<<pPhys[2]<<" ";
	    timelevel_=timelevel;
	    writeFieldValues(*(*it),p,file);
	}
	file<<"\n";
    }
    long int currentPosition=file.tellp();
    file.seekp(numberOfNodesPosition);
    file<<numberOfNodes;
    file.seekp(numberOfElementsPosition);
    file<<numberOfElements;
    file.seekp(currentPosition);
    for(int i=1; i<=numberOfNodes; ++i) {
	file<<i<<" ";
	if(i%1024==0){//some fileviewers cant handle very long lines
	    file<<endl;
	}
    }
    file<<endl;
}
*/
template<>
void hpGemUIExtentions<3>::elementErrorIntegrand(const Base::HpgemUI< 3 >::ElementT* element, const PointElementReferenceT& p, Matrix& ret){
    ret.resize(2,1);
    ElementInfos* info = static_cast<ElementInfos*>(const_cast<ElementT*>(element)->getUserData());
    PointPhysicalT pPhys;
    element->referenceToPhysical(p,pPhys);
    std::vector<NumericalVector> functionValues,functionCurls;
    info->makeFunctionValuesVector(element,p,functionValues);
    info->makeFunctionCurlsVector(element,p,functionCurls);
    
    NumericalVector phi(3),phiCurl(3),error(3),errorCurl(3);

    exactSolution(pPhys,measureTimes_[timelevel_],error);
    exactSolutionCurl(pPhys,measureTimes_[timelevel_],errorCurl);
    LinearAlgebra::Matrix data = element->getTimeLevelData(timelevel_);

    for(int i=0; i<element->getNrOfBasisFunctions(); ++i) {
	phi=functionValues[i];
	phiCurl=functionCurls[i];
	error-=data(i,0)*phi;
	errorCurl-=data(i,0)*phiCurl;
    }
    ret[0]=Base::L2Norm<3>(error)*Base::L2Norm<3>(error);
    ret[1]=Base::L2Norm<3>(errorCurl)*Base::L2Norm<3>(errorCurl);
}

template<>
void hpGemUIExtentions<3>::faceErrorIntegrand(const Base::HpgemUI< 3 >::FaceT* face, const Base::HpgemUI< 3 >::PointPhysicalT& normal, const PointFaceReferenceT& p, Matrix& ret) {
    ElementT* element=const_cast<ElementT*>(face->getPtrElementLeft());
    ElementInfos* info = static_cast<ElementInfos*>(element->getUserData());
    PointElementReferenceT pElement;
    PointPhysicalT PPhys;
    face->mapRefFaceToRefElemL(p,pElement);
    element->referenceToPhysical(pElement,PPhys);
    NumericalVector error(3),phi(3),dummy(3),normedNormal(3);
    normedNormal[0] = (normal*(1/Base::L2Norm<3>(normal)))[0];
    normedNormal[1] = (normal*(1/Base::L2Norm<3>(normal)))[1];
    normedNormal[2] = (normal*(1/Base::L2Norm<3>(normal)))[2];
    exactSolution(PPhys,measureTimes_[timelevel_],dummy);
    OuterProduct(normedNormal,dummy,error);
    std::vector<NumericalVector> functionValues;
    info->makeFunctionValuesVector(element,pElement,functionValues);
    LinearAlgebra::Matrix data = element->getTimeLevelData(timelevel_);
    for(int i=0; i<element->getNrOfBasisFunctions(); ++i) {
	dummy=functionValues[i];
	OuterProduct(normedNormal,dummy,phi);
	error-=data(i,0)*phi;
    }
    if(face->isInternal()) {
	element=const_cast<ElementT*>(face->getPtrElementRight());
	ElementInfos* infoAlso = static_cast<ElementInfos*>(element->getUserData());
	face->mapRefFaceToRefElemR(p,pElement);
	element->referenceToPhysical(pElement,PPhys);
	exactSolution(PPhys,measureTimes_[timelevel_],dummy);
	OuterProduct(normedNormal,dummy,phi);
	error-=phi;
	std::vector<NumericalVector> functionValuesToo;
	infoAlso->makeFunctionValuesVector(element,pElement,functionValuesToo);
	data=element->getTimeLevelData(timelevel_);
	for(int i=0; i<element->getNrOfBasisFunctions();++i) {
	    dummy=functionValuesToo[i];
	    OuterProduct(normedNormal,dummy,phi);
	    error+=data(i,0)*phi;//iets van een procentje te weinig
	}
    }
    ret[0]=Base::L2Norm<3>(error)*Base::L2Norm<3>(error);
}

template<>
void hpGemUIExtentions<3>::computeErrors(Base::HpgemUI< 3 >::MeshManipulatorT& Mesh, int timelevel, double& L2Norm, double& InfNorm, double& HCurlNorm, double& DGNorm){
    //this should probalby be toggled off when the exact solution is not known
    //alternatively something based on richardson extrapolation can be done
    int TotalAmountOfProcessors,localProcessorNumber,numberOfElements;
    MPI_Comm_size(PETSC_COMM_WORLD,&TotalAmountOfProcessors);
    MPI_Comm_rank(PETSC_COMM_WORLD,&localProcessorNumber);
    numberOfElements=meshes_[0]->getElementsList().size();
  
    ElementFunction elF = &hpGemUIExtentions::elementErrorIntegrand;
    this->timelevel_=timelevel;
    LinearAlgebra::Matrix integrationResults(2,1);
    Integration::ElementIntegral<3> elIntegral(false);
    L2Norm=0;
    HCurlNorm=0;
    for(ElementIterator it=Mesh.elementColBegin(); it!=Mesh.elementColEnd(); ++it) {
	if(((*it)->getID()-1)/(numberOfElements/TotalAmountOfProcessors+1)==localProcessorNumber) {
	    elIntegral.integrate((*it),elF,integrationResults,this);
	    L2Norm+=std::abs(integrationResults[0]);
	    HCurlNorm+=std::abs(integrationResults[0])+std::abs(integrationResults[1]);
	    integrationResults[0]=0;
	    integrationResults[1]=0;
	}
    }
    DGNorm=HCurlNorm;
    FaceFunction faF = & hpGemUIExtentions::faceErrorIntegrand;
    integrationResults.resize(1,1);
    Integration::FaceIntegral<3> faIntegral(false);
    for(FaceIterator it=Mesh.faceColBegin(); it!=Mesh.faceColEnd(); ++it) {
	if(((*it)->getPtrElementLeft()->getID()-1)/(numberOfElements/TotalAmountOfProcessors+1)==localProcessorNumber){
	    faIntegral.integrate(*it,faF,integrationResults,this);
	    DGNorm+=std::abs(integrationResults[0]);
	    integrationResults[0]=0;
	}
    }
}

template<>
void hpGemUIExtentions<3>::makeOutput(char* filename) {
    //set up containers for the errors
    Vec L2Norm, InfNorm, HCurlNorm, DGNorm;
    double L2NormEntry, InfNormEntry, HCurlNormEntry, DGNormEntry;
    ierr_=VecCreate(PETSC_COMM_WORLD,&L2Norm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetSizes(L2Norm,PETSC_DECIDE,getData()->numberOfTimeLevels_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetUp(L2Norm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicate(L2Norm,&HCurlNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicate(L2Norm,&DGNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    //do tecplot related stuff
    std::ofstream fileWriter;
    fileWriter.open(filename);
    int dimensions[3]= {0,1,2};
    Output::TecplotDiscontinuousSolutionWriter<3> tecplotWriter(fileWriter,"The electric field","012","E0,E1,E2,H0,H1,H2");
    std::stringstream string;
    char parsed[20]="";
    string.readsome(parsed,20);//clean storage
    string<<"t="<<measureTimes_[0];
    int read=string.readsome(parsed,20);
    string.readsome(&parsed[read],20-read);//readsome doen't want to read in one go on the first try :(
    writeFunction writeDataFunction=&hpGemUIExtentions<3>::writeFieldValues;
    timelevel_=0;
    tecplotWriter.write(meshes_[0],parsed,false,writeDataFunction,this);
    //writeTecplotFile(*meshes_[0],parsed,0,fileWriter,false);

    //compute errors
    computeErrors(*meshes_[0],0,L2NormEntry,InfNormEntry,HCurlNormEntry,DGNormEntry);
    VecSetValue(L2Norm,0,L2NormEntry,ADD_VALUES);
    VecSetValue(HCurlNorm,0,HCurlNormEntry,ADD_VALUES);
    VecSetValue(DGNorm,0,DGNormEntry,ADD_VALUES);
    for(int i=0; i<getData()->numberOfTimeLevels_-1; ++i) {
	timelevel_=i;
	computeErrors(*meshes_[0],i+1,L2NormEntry,InfNormEntry,HCurlNormEntry,DGNormEntry);//also updates the timelevel_
	VecSetValue(L2Norm,i+1,L2NormEntry,ADD_VALUES);
	VecSetValue(HCurlNorm,i+1,HCurlNormEntry,ADD_VALUES);
	VecSetValue(DGNorm,i+1,DGNormEntry,ADD_VALUES);
	string<<"t="<<measureTimes_[i+1];
	string.readsome(parsed,20);
	tecplotWriter.write(meshes_[0],parsed,true,writeDataFunction,this);
	//writeTecplotFile(*meshes_[0],parsed,i+1,fileWriter,true);
    }
    ierr_=VecAssemblyBegin(L2Norm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecAssemblyBegin(HCurlNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecAssemblyBegin(DGNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecAssemblyEnd(L2Norm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecAssemblyEnd(HCurlNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecAssemblyEnd(DGNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSqrtAbs(L2Norm);
    ierr_=VecSqrtAbs(HCurlNorm);
    ierr_=VecSqrtAbs(DGNorm);
    VecView(L2Norm,0);
//	VecView(L2Norm,PETSC_VIEWER_DRAW_WORLD);
    //VecView(InfNorm,0);//FIXME not computed yet
//	VecView(InfNorm,PETSC_VIEWER_DRAW_WORLD);
    VecView(HCurlNorm,0);
//	VecView(HCurlNorm,PETSC_VIEWER_DRAW_WORLD);
    VecView(DGNorm,0);
//	VecView(DGNorm,PETSC_VIEWER_DRAW_WORLD);
    ierr_=VecDestroy(&HCurlNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&DGNorm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&L2Norm);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
}

template<>
hpGemUIExtentions<3>::hpGemUIExtentions(int argc, char** argv, MaxwellData* globalConfig, Base::ConfigurationData* elementConfig, matrixFiller* fluxType):HpgemUI< 3 >(globalConfig,elementConfig), assembler(fluxType){
    ierr_=SlepcInitialize(&argc,&argv,(char*)0,"PETSc help");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=PetscLogBegin();CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCreate(PETSC_COMM_WORLD,&x_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCreate(PETSC_COMM_WORLD,&RHS_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCreate(PETSC_COMM_WORLD,&derivative_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatCreate(PETSC_COMM_WORLD,&M_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatCreate(PETSC_COMM_WORLD,&S_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatSetType(M_,"mpibaij");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatSetType(S_,"mpibaij");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=KSPCreate(PETSC_COMM_WORLD,&solver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=EPSCreate(PETSC_COMM_WORLD,&eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    storage_=new const PetscScalar*;
    measureTimes_=new double[getConfigData()->numberOfTimeLevels_];
}

template<>
hpGemUIExtentions<3>::~hpGemUIExtentions() {
    delete storage_;
    delete[] measureTimes_;
    ierr_=VecDestroy(&x_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&RHS_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&derivative_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatDestroy(&M_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatDestroy(&S_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=KSPDestroy(&solver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=EPSDestroy(&eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    PetscViewer log;
    ierr_=PetscViewerASCIIOpen(PETSC_COMM_WORLD,"maxwell.log",&log);
    ierr_=PetscLogView(log);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=PetscViewerDestroy(&log);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=SlepcFinalize();CHKERRABORT(PETSC_COMM_WORLD,ierr_);
}

template<>
unsigned int hpGemUIExtentions<3>::addMesh(Base::HpgemUI< 3 >::MeshManipulatorT* mesh){
    meshes_.push_back(mesh);
    return meshes_.size()-1;
}

template<>
void hpGemUIExtentions<3>::makeShiftMatrix(NumericalVector& direction, Vec& waveVecMatrix) {
    for(ElementIterator it=meshes_[0]->elementColBegin(); it!=meshes_[0]->elementColEnd(); ++it) {
	for(int j=0; j<(*it)->getNrOfBasisFunctions(); ++j) {
	    PointPhysicalT centerPhys;
	    PointReferenceT center;
	    (*it)->getReferenceGeometry()->getCenter(center);;
	    (*it)->referenceToPhysical(center,centerPhys);
	    //this extra accuracy is probably irrelevant and a lot of extra ugly to get it working
	    //static_cast<Base::threeDBasisFunction*>((*it)->basisFunctionSet_->vecOfBasisFcn_[j])->getReasonableNode(*(*it),centerPhys);
	    
	    PetscScalar value=exp(std::complex<double>(0,direction[0]*centerPhys[0]+direction[1]*centerPhys[1]+direction[2]*centerPhys[2]));
	    VecSetValue(waveVecMatrix,((*it)->getID()-1)*(*it)->getNrOfBasisFunctions()+j,value,INSERT_VALUES);
	}
    }
}

template<>
void hpGemUIExtentions<3>::findBoundaryBlocks(vector< IS >& xRow, vector< IS >& xCol, vector< IS >& yRow, vector< IS >& yCol, vector< IS >& zRow, vector< IS >& zCol){
    int nx(0),ny(0),nz(0);
    int places[]= {0};
    int blocksize=0;
    for(FaceIterator it=meshes_[0]->faceColBegin(); it!=meshes_[0]->faceColEnd(); ++it) {
	assert((*it)->isInternal());
	PointFaceReferenceT p;
	(*it)->getReferenceGeometry()->getCenter(p);
	PointElementReferenceT pLeft,pRight;
	PointPhysicalT pLeftPhys,pRightPhys;
	(*it)->mapRefFaceToRefElemL(p,pLeft);
	(*it)->mapRefFaceToRefElemR(p,pRight);
	(*it)->getPtrElementLeft()->referenceToPhysical(pLeft,pLeftPhys);
	(*it)->getPtrElementRight()->referenceToPhysical(pRight,pRightPhys);
	//if the left coordinate is not close to the right coordinate it is a boundary face
	if(Base::L2Norm(pLeftPhys-pRightPhys)>1e-3){//pretty lousy tolerance, but this norm should be either 1 or 0
	    //cout<<pLeftPhys<<endl<<pRightPhys<<endl<<pLeftPhys-pRightPhys<<endl<<(pLeftPhys[0]-pRightPhys[0])*(pLeftPhys[0]-pRightPhys[0])<<endl;;
	    if((pLeftPhys[0]-pRightPhys[0])*(pLeftPhys[0]-pRightPhys[0])>1e-3) {
	        //cout<<"X!"<<endl;
		xRow.resize(nx+2);
		xCol.resize(nx+2);
		places[0]=(*it)->getPtrElementLeft()->getID()-1;
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementLeft()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&xRow[nx]);
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementLeft()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&xCol[nx+1]);
		places[0]=(*it)->getPtrElementRight()->getID()-1;
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementRight()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&xRow[nx+1]);
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementRight()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&xCol[nx]);
		blocksize=(*it)->getPtrElementLeft()->getNrOfBasisFunctions();
		nx+=2;
	    }else if((pLeftPhys[1]-pRightPhys[1])*(pLeftPhys[1]-pRightPhys[1])>1e-3) {
	        //cout<<"Y!"<<endl;
		yRow.resize(ny+2);
		yCol.resize(ny+2);
		places[0]=(*it)->getPtrElementLeft()->getID()-1;
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementLeft()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&yRow[ny]);
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementLeft()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&yCol[ny+1]);
		places[0]=(*it)->getPtrElementRight()->getID()-1;
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementRight()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&yRow[ny+1]);
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementRight()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&yCol[ny]);
		ny+=2;
	    } else {
	        //cout<<"Z!"<<endl;
		zRow.resize(nz+2);
		zCol.resize(nz+2);
		assert((pLeftPhys[2]-pRightPhys[2])*(pLeftPhys[2]-pRightPhys[2])>(1.e-3));
		places[0]=(*it)->getPtrElementLeft()->getID()-1;
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementLeft()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&zRow[nz]);
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementLeft()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&zCol[nz+1]);
		places[0]=(*it)->getPtrElementRight()->getID()-1;
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementRight()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&zRow[nz+1]);
		ISCreateBlock(PETSC_COMM_WORLD,(*it)->getPtrElementRight()->getNrOfBasisFunctions(),1,places,PETSC_COPY_VALUES,&zCol[nz]);
		nz+=2;
	    }
	}
    }
}

template<>
void hpGemUIExtentions<3>::LDOSIntegrand(const Base::HpgemUI< 3 >::ElementT* element, const PointElementReferenceT& p, double& ret)
{//currently LDOS is computed by evaluation at a point so integrand is a bit of a misnomer
    ElementInfos* info = static_cast<ElementInfos*>(const_cast<ElementT*>(element)->getUserData());
    NumericalVector PhiRealI(3),PhiRealJ(3),PhiImagI(3),PhiImagJ(3);
    std::vector<NumericalVector> functionValues(element->getNrOfBasisFunctions());
    info->makeFunctionValuesVector(element,p,functionValues);
    for(int i=0;i<element->getNrOfBasisFunctions();++i){
	PhiRealI+=functionValues[i]*element->getData(0,0,i);
	PhiImagI+=functionValues[i]*element->getData(1,0,i);
	PhiRealJ+=functionValues[i]*element->getData(2,0,i);
	PhiImagJ+=functionValues[i]*element->getData(3,0,i);
    }
    //current implementation: orientation averaged LDOS; modify the next expression for other orientations
    ret=PhiRealI[0]*PhiRealJ[0]+PhiRealI[1]*PhiRealJ[1]+PhiRealI[2]*PhiRealJ[2]+PhiImagI[0]*PhiImagJ[0]+PhiImagI[1]*PhiImagJ[1]+PhiImagI[2]*PhiImagJ[2];
    ret*=48;//assume computation is done only in the irreducible brillouin zone
    //cout<<ret<<endl;
}


template<>
void hpGemUIExtentions<3>::makeFunctionValue(Vec eigenVector, NumericalVector& result){
    Vec scaledVec;
    double partialResult;
    int index(0);
    ierr_=VecDuplicate(eigenVector,&scaledVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatMult(M_,eigenVector,scaledVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    result.resize(4*meshes_[0]->getNumberOfElements());
    for(ElementIterator it=meshes_[0]->elementColBegin(); it!=meshes_[0]->elementColEnd(); ++it) {
	ierr_=VecGetArrayRead(eigenVector,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	for(int i=0;i<(*it)->getNrOfBasisFunctions();++i){
	    (*it)->setData(0,0,i,(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+i].real());
	    (*it)->setData(1,0,i,(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+i].imag());
	}
	ierr_=VecRestoreArrayRead(eigenVector,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecGetArrayRead(scaledVec,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	for(int i=0;i<(*it)->getNrOfBasisFunctions();++i){
	    (*it)->setData(2,0,i,(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+i].real());
	    (*it)->setData(3,0,i,(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+i].imag());
	}
	ierr_=VecRestoreArrayRead(scaledVec,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	for(int i=0;i<4;++i){
	    PointElementReferenceT p;
	    (*it)->getReferenceGeometry()->getCenter(p);
	    LDOSIntegrand(*it,p,partialResult);
	    result[index]=partialResult;
	    ++index;
	}
    }    
    ierr_=VecDestroy(&scaledVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
}

template<>
void hpGemUIExtentions<3>::solveTimeDependant(){
    cout<<"doing a time dependent simulation"<<endl;
    const MaxwellData* actualdata = getData();
    MHasToBeInverted_=true;
    assembler->fillMatrixes(this);

    Vec dummy,dummy2;
    ierr_=VecDuplicate(derivative_,&dummy);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicate(derivative_,&dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatMult(M_,derivative_,dummy);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCopy(dummy,derivative_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatMult(M_,x_,dummy);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCopy(dummy,x_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    double tau=0.05/( (2*actualdata->PolynomialOrder_+1)*actualdata->NumberOfIntervals_);
    double Nsteps=floor((actualdata->EndTime_-actualdata->StartTime_)/tau)+1;
    tau=(actualdata->EndTime_-actualdata->StartTime_)/Nsteps;
    Nsteps=(actualdata->EndTime_-actualdata->StartTime_)/tau;
    tau=(actualdata->EndTime_-actualdata->StartTime_)/Nsteps;
    Nsteps=(actualdata->EndTime_-actualdata->StartTime_)/tau;

    int measureStep=double(Nsteps)/double(getData()->numberOfTimeLevels_-1)+1;//round up

    double t=actualdata->StartTime_;
    measureTimes_[0]=t;

    double eta=sourceTermTime(t);
    ierr_=VecScale(RHS_,1/eta);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    double scale0=(1-actualdata->Sigma_*tau/2);
    double scale1=1/(1+actualdata->Sigma_*tau/2);

    int measureAmount=0;

    cout<<tau<<" "<<Nsteps<<endl;

    for(int i=0; i<Nsteps; ++i) {
	if(i%measureStep==0) {
	    //getArrayRead only gets local values, so make the entire vector local
	    Vec dummy4;
	    IS ISallNumbers;
	    int zero[]= {0};
	    ierr_=ISCreateBlock(PETSC_COMM_SELF,getData()->numberOfUnknowns_,1,zero,PETSC_COPY_VALUES,&ISallNumbers);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecCreateSeq(PETSC_COMM_SELF,getData()->numberOfUnknowns_,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecGetSubVector(x_,ISallNumbers,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecGetArrayRead(dummy4,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    for(ElementIterator it=meshes_[0]->elementColBegin(); it!=meshes_[0]->elementColEnd(); ++it) {
		for(int j=0; j<(*it)->getNrOfBasisFunctions(); ++j) {
		    //remnant from before timelevelData was nice and allowed you to set things, should be made less ugly
		    const_cast<LinearAlgebra::Matrix*>(&(*it)->getTimeLevelData(measureAmount))->operator[](j)=(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+j].real();
		}
	    }
	    measureTimes_[measureAmount]=t;
	    measureAmount++;
	    ierr_=VecRestoreArrayRead(dummy4,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}

	//leap-frog sceme (Yee)
	ierr_=VecAXPY(x_,tau/2,derivative_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=MatMult(S_,x_,dummy);CHKERRABORT(PETSC_COMM_WORLD,ierr_);           //starting here:dummy contaings partial update of x

	eta=sourceTermTime(t);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecCopy(RHS_,dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);      //starting here: dummy2 contains time-scaled version of RHS
	ierr_=VecScale(dummy2,0.5*tau*eta);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecAYPX(dummy,-tau,dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	eta=sourceTermTime(t+tau);
	ierr_=VecCopy(RHS_,dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecAXPY(dummy,0.5*tau*eta,dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecScale(dummy,scale1);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=MatMult(M_,dummy,dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);   //starting here: dummy2 contiains partial update of x
	ierr_=VecAYPX(derivative_,scale0*scale1,dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecAXPY(x_,tau/2,derivative_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	t=t+tau;
    }
    //getArrayRead only gets local values, so make the entire vector local
    Vec dummy4;
    IS ISallNumbers;
    int zero[]= {0};
    ierr_=ISCreateBlock(PETSC_COMM_SELF,getData()->numberOfUnknowns_,1,zero,PETSC_COPY_VALUES,&ISallNumbers);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCreateSeq(PETSC_COMM_SELF,getData()->numberOfUnknowns_,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecGetSubVector(x_,ISallNumbers,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecGetArrayRead(dummy4,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    for(ElementIterator it=meshes_[0]->elementColBegin(); it!=meshes_[0]->elementColEnd(); ++it) {
	for(int j=0; j<(*it)->getNrOfBasisFunctions(); ++j) {
	    const_cast<LinearAlgebra::Matrix*>(&(*it)->getTimeLevelData(measureAmount))->operator[](j)=(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+j].real();
	}
    }
    measureTimes_[measureAmount]=t;
    VecRestoreArrayRead(dummy4,storage_);
    ierr_=VecDestroy(&dummy);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&dummy2);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
}

template<>
void hpGemUIExtentions<3>::solveHarmonic(){
    cout<<"finding a time-harmonic solution"<<endl;
    const MaxwellData* actualdata = getData();

    MHasToBeInverted_=false;
    assembler->fillMatrixes(this);

    PC preconditioner;
    ierr_=KSPGetPC(solver_,&preconditioner);
    ierr_=PCSetType(preconditioner,"jacobi");
    ierr_=KSPSetPC(solver_,preconditioner);

    ierr_=MatAXPY(S_,-1,M_,SUBSET_NONZERO_PATTERN);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    ierr_=KSPSetTolerances(solver_,1e-10,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    ierr_=KSPSetType(solver_,"minres");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
//	ierr=MatConvert(S,"aij",MAT_REUSE_MATRIX,&S);CHKERRABORT(PETSC_COMM_WORLD,ierr);

    //everything that is set in the code, but before this line is overridden by command-line options
    ierr_=KSPSetFromOptions(solver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    //everything that is set in the code, but after this line overrides the comand-line options

    ierr_=KSPSetOperators(solver_,S_,S_,DIFFERENT_NONZERO_PATTERN);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=KSPSetUp(solver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=KSPSolve(solver_,RHS_,x_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    //getArrayRead only gets local values, so make the entire vector local
    Vec dummy;
    IS ISallNumbers;
    int zero[]= {0};
    ierr_=ISCreateBlock(PETSC_COMM_SELF,getData()->numberOfUnknowns_,1,zero,PETSC_COPY_VALUES,&ISallNumbers);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCreateSeq(PETSC_COMM_SELF,getData()->numberOfUnknowns_,&dummy);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecGetSubVector(x_,ISallNumbers,&dummy);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecGetArrayRead(dummy,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    for(ElementIterator it=meshes_[0]->elementColBegin(); it!=meshes_[0]->elementColEnd(); ++it) {
	for(int j=0; j<(*it)->getNrOfBasisFunctions(); ++j) {
	    const_cast<LinearAlgebra::Matrix*>(&(*it)->getTimeLevelData(0))->operator[](j)=(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+j].real();
	}
    }
    measureTimes_[0]=0;
    ierr_=VecRestoreArrayRead(x_,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
}

template<>
void hpGemUIExtentions<3>::solveEigenvalues(){
    cout<<"finding a bunch of eigenvalues"<<endl;
    const MaxwellData* actualdata = getData();
    int degreesOfFreedomPerElement=meshes_[0]->collBasisFSet_[0]->size();
    valarray<PetscScalar> blockvalues(degreesOfFreedomPerElement*degreesOfFreedomPerElement);
    int measureAmount=0;
    
    std::vector<IS> xboundaryRow,xboundaryCol,yboundaryRow,yboundaryCol,zboundaryRow,zboundaryCol;
    findBoundaryBlocks(xboundaryRow,xboundaryCol,yboundaryRow,yboundaryCol,zboundaryRow,zboundaryCol);

    //For IP-DG solving a general eigenproblem is slightly faster,
    //but the Brezzi formulation needs an inverse of each block in the mass matrix anyway
    //so there the general eigenproblem is just more work
    MHasToBeInverted_=true;
    assembler->fillMatrixes(this);

    // optionally divide the eigenvalues by pi^2 (for debugging / visual error tracking)
    //ierr_=MatScale(M_,1/M_PI/M_PI);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    //The eigensolver doesn't like block structures
    ierr_=MatConvert(M_,"aij",MAT_REUSE_MATRIX,&M_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatConvert(S_,"aij",MAT_REUSE_MATRIX,&S_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    Mat product,dummy;
    ierr_=MatMatMult(M_,S_,MAT_INITIAL_MATRIX,1.0,&product);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    //ierr_=EPSSetProblemType(eigenSolver_,EPS_HEP);
    ierr_=EPSSetOperators(eigenSolver_,product,NULL);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=EPSSetUp(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=EPSSetDimensions(eigenSolver_,24,PETSC_DECIDE,PETSC_DECIDE);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=EPSSetType(eigenSolver_,"jd");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ST transformation;
// 	KSP spectralProblem;
// 	PC preconditioner;
// 	ierr_=EPSGetST(eigenSolver_,&transformation);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=STSetType(transformation,"precond");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=STGetKSP(transformation,&spectralProblem);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=KSPGetPC(spectralProblem,&preconditioner);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=PCSetType(preconditioner,"ilu");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=EPSSetST(eigenSolver_,transformation);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    //everything that is set in the code, but before this line is overridden by command-line options
    ierr_=EPSSetFromOptions(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    //everything that is set in the code, but after this line overrides the comand-line options
    
    ierr_=EPSSolve(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    
    
    //if you are only interested in the default set of eigenvalues use this bit of code for output
//         ierr_=EPSPrintSolution(eigenSolver_,0);
//         CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 
// 	ierr_=EPSGetEigenvector(eigenSolver_,1,x_,derivative_);//actually eps,nr,real(x),imag(x)
// 	ierr_=VecGetArrayRead(x_,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	for(ElementIterator it=meshes_[0]->elementColBegin();it!=meshes_[0]->elementColEnd();++it){
// 	    for(int j=0;j<(*it)->getNrOfBasisFunctions();++j){
// 		const_cast<LinearAlgebra::Matrix*>(&(*it)->getTimeLevelData(0))->operator[](j)=(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+j].real();
// 	    }
// 	}
// 	measureTimes_[0]=0;
// 	ierr_=VecRestoreArrayRead(x_,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    

    //if you want all possible eigenvalues (including using the wave-vector) use this 'bit' of code
    //create some storage vectors
    PetscScalar neededOnlyForRealPetsc,eigenvalue;
    Vec *eigenvalues,example,*eigenVectors;
    eigenvalues=new Vec[24];
    eigenVectors=new Vec[40];//a few extra in case SLEPc finds more than the requested amount of eigenvalues
    ierr_=VecCreate(PETSC_COMM_WORLD,&example);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetSizes(example,PETSC_DECIDE,61);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetUp(example);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicateVecs(example,24,&eigenvalues);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicateVecs(x_,40,&eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&example);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    int converged;
    ierr_=EPSGetConverged(eigenSolver_,&converged);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    for(int i=0;i<converged&&i<24;++i){
	ierr_=EPSGetEigenvalue(eigenSolver_,i,&eigenvalue,&neededOnlyForRealPetsc);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecSetValue(eigenvalues[i],0,eigenvalue,ADD_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    }

    Vec waveVec,waveVecConjugate;
    const int *rows,*columns;
    ierr_=VecCreate(PETSC_COMM_WORLD,&waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetType(waveVec,"mpi");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetBlockSize(waveVec,meshes_[0]->collBasisFSet_[0]->size());
    ierr_=VecSetSizes(waveVec,PETSC_DECIDE,globalData_->numberOfUnknowns_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetUp(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    NumericalVector k(3);
    k[0]=M_PI/20.;k[1]=0;k[2]=0;
    makeShiftMatrix(k,waveVec);
    ierr_=VecAssemblyBegin(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecAssemblyEnd(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicate(waveVec,&waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCopy(waveVec,waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecConjugate(waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    PetscLayout distribution;
    int localProcessorNumber,blockProcessorNumber,localsize;
    MPI_Comm_rank(PETSC_COMM_WORLD,&localProcessorNumber);
    ierr_=PetscLayoutCreate(PETSC_COMM_WORLD,&distribution);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatGetLocalSize(product,&localsize,NULL);
    ierr_=PetscLayoutSetLocalSize(distribution,localsize);
    ierr_=PetscLayoutSetUp(distribution);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    for(int i=1;i<61;++i){
	cout<<i<<endl;
	if(i==21){
	    //these are only increments, actually this make the wavevector move from pi,0,0 to pi,pi,0
	    k[0]=0;k[1]=M_PI/20.;

	    //recompute the shifts
	    makeShiftMatrix(k,waveVec);
	    ierr_=VecAssemblyBegin(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecAssemblyEnd(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecCopy(waveVec,waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecConjugate(waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}else if(i==41){
	    k[1]=0;k[2]=M_PI/20.;
	    makeShiftMatrix(k,waveVec);
	    ierr_=VecAssemblyBegin(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecAssemblyEnd(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecCopy(waveVec,waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecConjugate(waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
	ierr_=EPSGetInvariantSubspace(eigenSolver_,eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=MatDiagonalScale(product,waveVec,waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

	//this is probably not the best way to do this
	//a cleaner set-up can be made using two iterators
	for(int j=0;j<xboundaryCol.size();++j){
	    ierr_=ISGetIndices(xboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISGetIndices(xboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    PetscLayoutFindOwner(distribution,rows[0],&blockProcessorNumber);
	    //if(blockProcessorNumber==localProcessorNumber){
		ierr_=MatGetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		blockvalues*=exp(std::complex<double>(0,k[0]*(j%2==0?-1:1)));
		ierr_=MatSetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0],INSERT_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    //}
	    ierr_=MatAssemblyBegin(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=MatAssemblyEnd(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(xboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(xboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
	for(int j=0;j<yboundaryCol.size();++j){
	    ierr_=ISGetIndices(yboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISGetIndices(yboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    PetscLayoutFindOwner(distribution,rows[0],&blockProcessorNumber);
	    //if(blockProcessorNumber==localProcessorNumber){
		ierr_=MatGetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		blockvalues*=exp(std::complex<double>(0,k[1]*(j%2==0?-1:1)));
		ierr_=MatSetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0],INSERT_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    //}
	    ierr_=MatAssemblyBegin(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=MatAssemblyEnd(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(yboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(yboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
	for(int j=0;j<zboundaryCol.size();++j){
	    ierr_=ISGetIndices(zboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISGetIndices(zboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    PetscLayoutFindOwner(distribution,rows[0],&blockProcessorNumber);
	    //if(blockProcessorNumber==localProcessorNumber){
		ierr_=MatGetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		blockvalues*=exp(std::complex<double>(0,k[2]*(j%2==0?-1:1)));
		ierr_=MatSetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0],INSERT_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    //}
	    ierr_=MatAssemblyBegin(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=MatAssemblyEnd(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(zboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(zboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
	
	
	//outputs 'old' data
	if(i%20==1) {
	    //getArrayRead only gets local values, so make the entire vector local
	    Vec dummy4;
	    IS ISallNumbers;
	    int zero[]= {0};
	    ierr_=ISCreateBlock(PETSC_COMM_SELF,getData()->numberOfUnknowns_,1,zero,PETSC_COPY_VALUES,&ISallNumbers);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecCreateSeq(PETSC_COMM_SELF,getData()->numberOfUnknowns_,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    for(int j=0;j<10&&j<converged;++j){
		ierr_=VecGetSubVector(eigenVectors[j],ISallNumbers,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		ierr_=VecGetArrayRead(dummy4,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		for(ElementIterator it=meshes_[0]->elementColBegin(); it!=meshes_[0]->elementColEnd(); ++it) {
		    for(int k=0; k<(*it)->getNrOfBasisFunctions(); ++k) {
			const_cast<LinearAlgebra::Matrix*>(&(*it)->getTimeLevelData(measureAmount))->operator[](k)=(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+k].real();
		    }
		}
		ierr_=VecRestoreArrayRead(dummy4,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		measureTimes_[measureAmount]=i/20;
		measureAmount++;
	    }
	}
	
	

	ierr_=EPSSetOperators(eigenSolver_,product,NULL);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSSetInitialSpace(eigenSolver_,converged,eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSSetUp(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSSolve(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSGetConverged(eigenSolver_,&converged);CHKERRABORT(PETSC_COMM_WORLD,ierr_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

	for(int j=0;j<converged&&j<24;++j){
	    ierr_=EPSGetEigenvalue(eigenSolver_,j,&eigenvalue,&neededOnlyForRealPetsc);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=VecSetValue(eigenvalues[j],i,eigenvalue,INSERT_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
    }
    for(int i=0;i<xboundaryCol.size();++i){
	ISDestroy(&xboundaryCol[i]);
	ISDestroy(&xboundaryRow[i]);
    }
    for(int i=0;i<yboundaryCol.size();++i){
	ISDestroy(&yboundaryCol[i]);
	ISDestroy(&yboundaryRow[i]);
    }
    for(int i=0;i<zboundaryCol.size();++i){
	ISDestroy(&zboundaryCol[i]);
	ISDestroy(&zboundaryRow[i]);
    }
    for(int i=0;i<20;++i){
	ierr_=VecAssemblyBegin(eigenvalues[i]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecAssemblyEnd(eigenvalues[i]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecView(eigenvalues[i],0);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    }
    
    ierr_=EPSGetInvariantSubspace(eigenSolver_,eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    
    Vec dummy4;
    IS ISallNumbers;
    int zero[]= {0};
    ierr_=ISCreateBlock(PETSC_COMM_SELF,getData()->numberOfUnknowns_,1,zero,PETSC_COPY_VALUES,&ISallNumbers);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCreateSeq(PETSC_COMM_SELF,getData()->numberOfUnknowns_,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    for(int j=0;j<10&&j<converged;++j){
	ierr_=VecGetSubVector(eigenVectors[j],ISallNumbers,&dummy4);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=VecGetArrayRead(dummy4,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	for(ElementIterator it=meshes_[0]->elementColBegin(); it!=meshes_[0]->elementColEnd(); ++it) {
	    for(int k=0; k<(*it)->getNrOfBasisFunctions(); ++k) {
		const_cast<LinearAlgebra::Matrix*>(&(*it)->getTimeLevelData(measureAmount))->operator[](k)=(*storage_)[(*it)->getNrOfBasisFunctions()*((*it)->getID()-1)+k].real();
	    }
	}
	ierr_=VecRestoreArrayRead(dummy4,storage_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	measureTimes_[measureAmount]=3;
	measureAmount++;
    }
    
    

    ierr_=VecDestroy(&waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    
    
    //always clean up after you are done
    ierr_=MatDestroy(&product);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
}

template<>
void hpGemUIExtentions<3>::solveDOS()
{
    cout<<"finding the local density of states"<<endl;
    const MaxwellData* actualdata = getData();
    int degreesOfFreedomPerElement=meshes_[0]->collBasisFSet_[0]->size();
    valarray<PetscScalar> blockvalues(degreesOfFreedomPerElement*degreesOfFreedomPerElement);
    int measureAmount=0;
    std::vector<double> eigenvalues;
    std::vector<NumericalVector> functionValues;
    std::vector<IS> xboundaryRow,xboundaryCol,yboundaryRow,yboundaryCol,zboundaryRow,zboundaryCol;
    findBoundaryBlocks(xboundaryRow,xboundaryCol,yboundaryRow,yboundaryCol,zboundaryRow,zboundaryCol);
    KspaceData brillouinZone(9);

    //For IP-DG solving a general eigenproblem is slightly faster,
    //but the Brezzi formulation needs an inverse of each block in the mass matrix anyway
    //so there the general eigenproblem is just more work
    MHasToBeInverted_=true;
    assembler->fillMatrixes(this);

    //Divide the eigenvalues by pi^2 (because some people cant do that by heart and the useful info shouldn't be hidden)
    //ierr_=MatScale(M_,1/M_PI/M_PI);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    //The eigensolver doesn't like block structures
    ierr_=MatConvert(M_,"aij",MAT_REUSE_MATRIX,&M_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatConvert(S_,"aij",MAT_REUSE_MATRIX,&S_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    Mat product,dummy;
    ierr_=MatMatMult(M_,S_,MAT_INITIAL_MATRIX,1.0,&product);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

//    ierr_=EPSSetProblemType(eigenSolver_,EPS_HEP);
    ierr_=EPSSetOperators(eigenSolver_,product,NULL);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=EPSSetUp(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=EPSSetDimensions(eigenSolver_,24,PETSC_DECIDE,PETSC_DECIDE);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=EPSSetType(eigenSolver_,"jd");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ST transformation;
// 	KSP spectralProblem;
// 	PC preconditioner;
// 	ierr_=EPSGetST(eigenSolver_,&transformation);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=STSetType(transformation,"precond");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=STGetKSP(transformation,&spectralProblem);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=KSPGetPC(spectralProblem,&preconditioner);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=PCSetType(preconditioner,"ilu");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
// 	ierr_=EPSSetST(eigenSolver_,transformation);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    //everything that is set in the code, but before this line is overridden by command-line options
    ierr_=EPSSetFromOptions(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    //everything that is set in the code, but after this line overrides the comand-line options
    
    PetscScalar target;    
    ierr_=EPSGetTarget(eigenSolver_,&target);CHKERRABORT(PETSC_COMM_WORLD,ierr_);    

    ierr_=EPSSolve(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    
    
    PetscScalar neededOnlyForRealPetsc,eigenvalue;
    Vec example,*eigenVectors;
    eigenVectors=new Vec[40];//a few extra in case SLEPc finds more than the requested amount of eigenvalues
    ierr_=VecCreate(PETSC_COMM_WORLD,&example);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetSizes(example,PETSC_DECIDE,61);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetUp(example);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicateVecs(x_,40,&eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&example);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    int converged;
    ierr_=EPSGetConverged(eigenSolver_,&converged);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    
    ierr_=EPSGetInvariantSubspace(eigenSolver_,eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    for(int i=0;i<converged&&i<24;++i){
	ierr_=EPSGetEigenvalue(eigenSolver_,i,&eigenvalue,&neededOnlyForRealPetsc);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	//SLEPSc sorts the eigenvalues by closest to the target first, this reorders them for the better smallest first
	//if SLEPSc manages to find some of the zero eigenvalues they should be skipped
	if(eigenvalue.real()>1e-6){
	    NumericalVector functionvalue;
	    //makeFunctionValue(eigenVectors[i],functionvalue);
	    if(eigenvalue.real()<target.real()){
		eigenvalues.insert(eigenvalues.begin(),sqrt(eigenvalue.real()));
		functionValues.insert(functionValues.begin(),functionvalue);
	    }else{
		eigenvalues.push_back(sqrt(eigenvalue.real()));
		functionValues.push_back(functionvalue);
	    }
	}
    }
    //eigenvalues.insert(eigenvalues.begin(),2,0);//at the infinite wavelength limit the two constant functions also contribute to the DOS
    brillouinZone.setOmega(eigenvalues);
    //brillouinZone.setFunctionValues(functionValues);

    Vec waveVec,waveVecConjugate;
    const int *rows,*columns;
    ierr_=VecCreate(PETSC_COMM_WORLD,&waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetType(waveVec,"mpi");CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetBlockSize(waveVec,meshes_[0]->collBasisFSet_[0]->size());
    ierr_=VecSetSizes(waveVec,PETSC_DECIDE,globalData_->numberOfUnknowns_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecSetUp(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    NumericalVector k(3);
    k[0]=M_PI/60;k[1]=0;k[2]=0;
    makeShiftMatrix(k,waveVec);
    ierr_=VecAssemblyBegin(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecAssemblyEnd(waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDuplicate(waveVec,&waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecCopy(waveVec,waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecConjugate(waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    PetscLayout distribution;
    int localProcessorNumber,blockProcessorNumber,localsize;
    MPI_Comm_rank(PETSC_COMM_WORLD,&localProcessorNumber);
    ierr_=PetscLayoutCreate(PETSC_COMM_WORLD,&distribution);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=MatGetLocalSize(product,&localsize,NULL);
    ierr_=PetscLayoutSetLocalSize(distribution,localsize);
    ierr_=PetscLayoutSetUp(distribution);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

    while(brillouinZone.hasNextPoint()){
	k=brillouinZone.nextPoint();
	ierr_=MatDiagonalScale(product,waveVec,waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

	//find a better strategy -- at least there is no communication in this set-up so MatAssemblyBegin should be reasonably fast
	//a cleaner set-up can be made using two iterators
	for(int j=0;j<xboundaryCol.size();++j){
	    ierr_=ISGetIndices(xboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISGetIndices(xboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    PetscLayoutFindOwner(distribution,rows[0],&blockProcessorNumber);
	    if(blockProcessorNumber==localProcessorNumber){
		ierr_=MatGetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		blockvalues*=exp(std::complex<double>(0,k[0]*(j%2==0?-1:1)));
		ierr_=MatSetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0],INSERT_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    }
	    ierr_=MatAssemblyBegin(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=MatAssemblyEnd(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(xboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(xboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
	for(int j=0;j<yboundaryCol.size();++j){
	    ierr_=ISGetIndices(yboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISGetIndices(yboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    PetscLayoutFindOwner(distribution,rows[0],&blockProcessorNumber);
	    if(blockProcessorNumber==localProcessorNumber){
		ierr_=MatGetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		blockvalues*=exp(std::complex<double>(0,k[1]*(j%2==0?-1:1)));
		ierr_=MatSetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0],INSERT_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    }
	    ierr_=MatAssemblyBegin(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=MatAssemblyEnd(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(yboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(yboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
	for(int j=0;j<zboundaryCol.size();++j){
	    ierr_=ISGetIndices(zboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISGetIndices(zboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    PetscLayoutFindOwner(distribution,rows[0],&blockProcessorNumber);
	    if(blockProcessorNumber==localProcessorNumber){
		ierr_=MatGetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0]);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
		blockvalues*=exp(std::complex<double>(0,k[2]*(j%2==0?-1:1)));
		ierr_=MatSetValues(product,degreesOfFreedomPerElement,rows,degreesOfFreedomPerElement,columns,&blockvalues[0],INSERT_VALUES);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    }
	    ierr_=MatAssemblyBegin(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=MatAssemblyEnd(product,MAT_FINAL_ASSEMBLY);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(zboundaryRow[j],&rows);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    ierr_=ISRestoreIndices(zboundaryCol[j],&columns);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	}
	

	ierr_=EPSSetOperators(eigenSolver_,product,NULL);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSSetInitialSpace(eigenSolver_,converged,eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSSetUp(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSSolve(eigenSolver_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSGetInvariantSubspace(eigenSolver_,eigenVectors);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	ierr_=EPSGetConverged(eigenSolver_,&converged);CHKERRABORT(PETSC_COMM_WORLD,ierr_);CHKERRABORT(PETSC_COMM_WORLD,ierr_);

	eigenvalues.clear();
	functionValues.clear();
	for(int j=0;j<converged&&j<24;++j){
	    ierr_=EPSGetEigenvalue(eigenSolver_,j,&eigenvalue,&neededOnlyForRealPetsc);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
	    if(eigenvalue.real()>1e-6){
		NumericalVector functionvalue;
		//makeFunctionValue(eigenVectors[j],functionvalue);
		if(eigenvalue.real()<target.real()){
		    eigenvalues.insert(eigenvalues.begin(),sqrt(eigenvalue.real()));
		    functionValues.insert(functionValues.begin(),functionvalue);
		}else{
		    eigenvalues.push_back(sqrt(eigenvalue.real()));
		    functionValues.push_back(functionvalue);
		}
	    }
	}
	brillouinZone.setOmega(eigenvalues);
	//brillouinZone.setFunctionValues(functionValues);
    }
    for(int i=0;i<xboundaryCol.size();++i){
	ISDestroy(&xboundaryCol[i]);
	ISDestroy(&xboundaryRow[i]);
    }
    for(int i=0;i<yboundaryCol.size();++i){
	ISDestroy(&yboundaryCol[i]);
	ISDestroy(&yboundaryRow[i]);
    }
    for(int i=0;i<zboundaryCol.size();++i){
	ISDestroy(&zboundaryCol[i]);
	ISDestroy(&zboundaryRow[i]);
    }   

    ierr_=VecDestroy(&waveVec);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    ierr_=VecDestroy(&waveVecConjugate);CHKERRABORT(PETSC_COMM_WORLD,ierr_);    
    
    //always clean up after you are done
    ierr_=MatDestroy(&product);CHKERRABORT(PETSC_COMM_WORLD,ierr_);
    
    NumericalVector result(meshes_[0]->getNumberOfElements());
    for(int i=0;i<1001;++i){
        brillouinZone.getIntegral(double(i)/100.,result);
        cout<<result[0]<<" "<<result[6]<<" "<<result[10]<<endl;
	result[0]=0;
	//result[6]=0;
	//result[10]=0;
    }
}
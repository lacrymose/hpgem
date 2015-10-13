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

#ifndef BASEEXTENDED_HPP
#define BASEEXTENDED_HPP

//This file contains all the fiddly bits about interpolation and filling matrices and stuff

#include "Base/MeshManipulator.h"
#include "Integration/ElementIntegrandBase.h"
#include "Integration/FaceIntegrandBase.h"
#include "Integration/ElementIntegral.h"
#include "Integration/FaceIntegral.h"
#include "Base/HpgemAPIBase.h"
//#include <Base/Norm2.h>
#include <petscksp.h>
#include <slepceps.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include "ElementInfos.h"
#include "Output/TecplotSingleElementWriter.h"
#include "Base/Element.h"
#include "Base/Face.h"
#include "Utilities/GlobalMatrix.h"
#include "Utilities/GlobalVector.h"
#include "petscksp.h"
#include "MatrixAssembly.h"
#include "LinearAlgebra/SmallVector.h"
#include "LinearAlgebra/MiddleSizeMatrix.h"
#include "LinearAlgebra/MiddleSizeVector.h"
#include "BasisFunctionCollection_Curl.h"


using basisFunctionT = Base::threeDBasisFunction;

using ElementT = Base::Element;
using FaceT = Base::Face;
using PointPhysicalT = Geometry::PointPhysical<DIM>;
using PointElementReferenceT = Geometry::PointReference<DIM>;
using PointFaceReferenceT = Geometry::PointReference<DIM>;
//typedef std::list<Base::Face<3>* >::iterator FaceIteratorT;

class MatrixAssembly;
namespace Integration
{

//Tell the integrators that these functions are valid as integrands

//face-integrand
/*template <template<unsigned int> class B, typename T>
 struct ReturnTrait1<void (B::*)(const FaceT*, const Geometry::PointPhysical<DIM>&, const PointFaceReferenceT&, T& )> {
 typedef T ReturnType;
 };
 
 //element-integrand
 template <template<unsigned int> class B, typename T>
 struct ReturnTrait1<void (B::*)(const ElementT*, const PointElementReferenceT&, T&)> {
 typedef T ReturnType;
 };*/
}

struct errorData
{
public:
    double& operator[](int n)
    {
        return data_[n];
    }
    void reset()
    {
        data_[0] = 0;
        data_[1] = 0;
    }
    errorData& operator*=(double rhs)
    {
        data_[0] *= rhs;
        data_[1] *= rhs;
        return *this;
    }
    ;
    void axpy(double a, const errorData& data)
    {
        data_[0] += a * data.data_[0];
        data_[1] += a * data.data_[1];
    }
    ;
private:
    double data_[2];
};

/**
 * This class provides some significant extentions to HpgemUI that may also be useful for other problems
 */
class hpGemUIExtentions : public Base::HpgemAPIBase<DIM>, Output::TecplotSingleElementWriter<DIM>, Integration::ElementIntegrandBase<errorData, DIM>, Integration::FaceIntegrandBase<errorData, DIM>
{
    
public:
    
    Vec x_, RHS_, derivative_;
    const PetscScalar** storage_;
    Mat M_, S_;
    bool MHasToBeInverted_;
    KSP solver_;
    EPS eigenSolver_;
    double* measureTimes_;
    int timelevel_; //this is a hack - passing a timelevel to the error integrand requires rewriting the actual integration routine
    PetscErrorCode ierr_;
    MatrixAssembly* assembler;

    /**
     * Makes sure the configuration data and the global data agree on the information they share
     */
    void setConfigData();

    //! gives read-only acces to the configuration data
    const Base::ConfigurationData* getConfigData();

    //! gives read-only acces to the global data
    const MaxwellData* getData() const;

    /**
     * set up the mesh and complete initialisation of the global data and the configuration data
     */
    //virtual bool initialise()=0;
    /**
     * this is where you specify the time part of the source Term
     * assumes that the source term can be split is a spatial part and a time part
     */

    static double sourceTermTime(const double t);

    /**
     * this is where you choose the solution of your problem
     * this will only have an effect on the accuracy of your error estimates
     * as a temporary solution remember to also update the exact solution in fillMatrices.cpp otherwise the final result will be incorrect
     */
    static void exactSolution(const Geometry::PointPhysical<DIM>& p, const double t, LinearAlgebra::SmallVector<DIM>& ret);

    /**
     * this is where you choose the curl of the solution of your problem
     * this will only have an effect on the accuracy of your error estimates
     */
    static void exactSolutionCurl(const Geometry::PointPhysical<DIM>& p, const double t, LinearAlgebra::SmallVector<DIM>& ret);

    //using ElementFunction = void(hpGemUIExtentions::*)(const ElementT*, const PointElementReferenceT&, LinearAlgebra::Matrix&);

    /**
     * integrand for the filling of the mass matrix M
     * \param [in] element the element that is currently being integrated on
     * \param [in] p the gauss point
     * \param [out] ret the contributions to the mass matrix from this point. This should not yet be scaled down with the weight of this point!
     */
    //virtual void elementIntegrand(const ElementT *element, const PointElementReferenceT &p, LinearAlgebra::Matrix &ret)=0;
    //virtual void elementStiffnessIntegrand(const ElementT *element, const PointElementReferenceT &p, LinearAlgebra::Matrix &ret)=0;
    //using FaceFunction = void (hpGemUIExtentions::*)(const FaceT* , const Geometry::PointPhysical<DIM>&, const PointFaceReferenceT&, LinearAlgebra::Matrix&); //check again
    
    /**
     * integrand for the filling of the face contibutions to the stiffness matrix S, without any penalty terms
     * \param [in] element the element that is currently being integrated on
     * \param [in] p the gauss point
     * \param [out] ret the contributions to the stifness matrix from this point. This should not yet be scaled down with the weight of this point!
     * For internal faces the integration expects that this matrix contains first contributions associated with the left element and then with the right element
     */
    //virtual void faceIntegrand(const FaceT *face, const LinearAlgebra::NumericalVector &normal, const PointFaceReferenceT &p, LinearAlgebra::Matrix &ret)=0;
    //virtual void faceIntegrandIPPart(const FaceT *face, const Geometry::PointPhysical<DIM> &normal, const PointFaceReferenceT &p, LinearAlgebra::Matrix &ret)=0;
    //virtual void faceIntegrandBRPart(const FaceT *face, const Geometry::PointPhysical<DIM> &normal, const PointFaceReferenceT &p, LinearAlgebra::Matrix &ret)=0;
    /**
     * provides the exact solution, for comparison purposes. If no exact solution is implemented this function produces nonsense.
     * \param [in] p the point where the exact solution is wanted
     * \param [in] t the time when the exact solution is wanted
     * \param [out] ret the exact field at p, accoriding to analytical predicions
     */
    //virtual void exactSolution(const Geometry::PointPhysical<DIM>& p, const double t, LinearAlgebra::NumericalVector &ret) {}
    /**
     * provides the curl of the exact solution, for comparison purposes. If no exact curl is implemented this function produces nonsense.
     * \param [in] p the point where the exact solution is wanted
     * \param [in] t the time when the exact solution is wanted
     * \param [out] ret the exact field at p, accoriding to analytical predicions
     */
    // virtual void exactSolutionCurl(const Geometry::PointPhysical<DIM>& p, const double t, LinearAlgebra::NumericalVector &ret) {}
    /**
     * provides the initial condition for the time dependent solution
     * \param [in] p the point where the initial condition is wanted
     * \param [out] ret the initial field at p
     */
    // virtual void initialConditions(const Geometry::PointPhysical<DIM> &p, LinearAlgebra::NumericalVector &ret)=0;
    /**
     * integrand used for the computation of the initial expansion coefficients
     * maps the reference point and the element to physical coordinates and then uses this to call the user provided initial conditions
     */
    //void elementIntegrand(const ElementT *element, const PointElementReferenceT &p, LinearAlgebra::NumericalVector &ret);
    /**
     * Computes the time-dependent part of the source term. This assumes that the source term can be uncoupled in a time dependent part and a space dependent part
     * \param [in] t The point in time where the source is to be evaluated.
     * \return The value of the source term.
     */
    //virtual double sourceTermTime(const double t)=0;
    //void sourceTerm(const ElementT *element, const PointElementReferenceT &p, LinearAlgebra::Matrix &ret);
    //void sourceTermBoundaryIP(const FaceT *face, const Geometry::PointPhysical<DIM> &normal, const PointFaceReferenceT &p, LinearAlgebra::Matrix &ret);
    /**
     * Integrand used for the computation of the boundary contributions to the RHS. This will be scaled by the same time dependent factor as in the source therm, just like in the original code by Domokos.
     * This version computes the terms that are common to both the Brezzi flux and the Interior Penalty flux
     */
    //void faceIntegrand(const FaceT *face, const LinearAlgebra::NumericalVector &normal, const PointFaceReferenceT &p, LinearAlgebra::NumericalVector &ret);
    //void sourceTermBoundaryBR(const FaceT *face, const Geometry::PointPhysical<DIM> &normal, const PointFaceReferenceT &p, LinearAlgebra::Matrix &ret);
    /**
     * Provides the time derivative of the initial solution
     * \param [in] p The point in space where the initial solution is to be evaluated.
     * \param [out] ret The time derivative of the source term.
     */
    //virtual void initialConditionsDeriv(const Geometry::PointPhysical<DIM> &p, LinearAlgebra::NumericalVector &ret)=0;
    /**
     * Integrand used for the computation of the time derivative of the initial solution
     * maps the reference point and the element to physical coordinates and then uses this to call the user provided initial conditions
     */
    // void initialConditionsDeriv(const ElementT *element, const PointElementReferenceT &p, LinearAlgebra::Matrix &ret);
    using writeFunction = void (hpGemUIExtentions::*)(const ElementT&,const PointElementReferenceT&,std::ostream&);

    /**
     * Expand the solution back from the expansion coefficients and write them to an output stream
     * \param [in] element,p the point where the solution should be computed, given as element/reference point pair.
     * \param [in] t the timelevel that is wanted in the output.
     * \param output an output stream ready to accept the solution values
     */
    
    void writeToTecplotFile(const Base::Element*, const PointReferenceT&, std::ostream&);

    //tecplotwriter is also avaiable in the hpGEM kernel
    //void writeTecplotFile(const MeshManipulatorT& mesh, const char* zonetitle, const int timelevel, std::ofstream& file, const bool existingFile);
    
    /**
     * Integrand for the computation of the L^2 and the Hcurl error
     */
    void elementIntegrand(Base::PhysicalElement<DIM>& el, errorData& ret);

    /**
     * Integrand for the comutation of the jump part of the DG error
     */
    void faceIntegrand(Base::PhysicalFace<DIM>& fa, errorData& ret);

    void LDOSIntegrand(Base::PhysicalElement<DIM>& el, double& ret);

    /**
     * Function for the computation of some usefull errors measures. Currently computes the L2 norm, the HCurl and the DG norm norm.
     * This function does not guarantee correct results if the exact solution is not known
     */
    void computeErrors(Base::MeshManipulator<DIM>& Mesh, int timelevel, double& L2Norm, double& InfNorm, double& HCurlNorm, double& DGNorm);

    /**
     * Writes a tecplot-readable file with the solution on some time-levels and uses PETSc to display the errors.
     * Assumes the solution is already available
     * \param [in] filename The name of the file that will contain the tecplot output.
     */
    void makeOutput(char* filename);

    /**
     * Constructor allows PETSc to parse any PETSc-related input and does most of the initialisations.
     *
     * This constructor is extremely bare bones: it only guarantees PETSc dataTypes exists, not that they are
     * actually usefull for computations or storing data.
     */
    hpGemUIExtentions(MaxwellData* globalConfig, Base::ConfigurationData* elementConfig, MatrixAssembly* fluxType);

    /**
     * Deconstructor cleans up again and logs performance statistics of PETSc
     */
    ~hpGemUIExtentions();

    /**
     * Wrapper for protected function in superclass
     */
    MeshId addMesh(Base::MeshManipulator<DIM>* mesh);

    /**
     * makes a matrix with the shifts
     */
    void makeShiftMatrix(LinearAlgebra::SmallVector<DIM>& direction, Vec& waveVecMatrix);

    /**
     * Tell PETSc the places where special care must be taken because of periodic boundary conditions
     * different sorts of care need to be taken in different cardinal directions so this function also
     * splits the boundary appropriately
     * this function takes vectors with enough space preallocated as arguments and fills them up
     * the 6 vecors are ordered as first x-direction, then y-direction, then z-direction and per direction
     * with rows first and then columns
     */
    void findBoundaryBlocks(std::vector<IS>& xRow, std::vector<IS>& xCol, std::vector<IS>& yRow, std::vector<IS>& yCol, std::vector<IS>& zRow, std::vector<IS>& zCol);

    /**
     * Call after setting up the mesh. This will arrange that the matrices are assembled and then solve Sx+omegaEpsilon*Mderivative=RHS using a leap-frog time-stepping method
     */
    void solveTimeDependant();

    /**
     * Call after setting up the mesh. This will arrange that the matrices are assembled and then solve Sx-omega*Mx=RHS using a krylov-subspace solver
     */
    void solveHarmonic();

    /**
     * Call after setting up the mesh. This will arrange that the matrices are assembled and then solve a series of eigenvalue problems in k-space
     */
    void solveEigenvalues();

    /*
     * Call after setting up the mesh. This will arrange that the matrices are assembled and then find the Density of States using eigenfunction expansions
     */
    void solveDOS();

    /**
     * If you just want to dump the matrixes for some testing in matlab, this is your routine
     */
    void exportMatrixes();

    /**
     * given an eigenvector, prepare an expression for f(x) in int(f(x)*delta(omega) dx)
     */
    void makeFunctionValue(Vec eigenVector, LinearAlgebra::MiddleSizeVector& result);

    class anonymous1 : public Integration::ElementIntegrandBase<LinearAlgebra::MiddleSizeMatrix, DIM>
    {
    public:
        
        void elementIntegrand(Base::PhysicalElement<DIM>& el , LinearAlgebra::MiddleSizeMatrix& ret);
    } elementMassIntegrand;

    class anonymous2 : public Integration::ElementIntegrandBase<LinearAlgebra::MiddleSizeMatrix, DIM>
    {
    public:
        
        void elementIntegrand(Base::PhysicalElement<DIM>& el , LinearAlgebra::MiddleSizeMatrix& ret);
    } elementStiffnessIntegrand;

    class anonymous3 : public Integration::ElementIntegrandBase<LinearAlgebra::MiddleSizeVector, DIM>
    {
    public:
        
        void elementIntegrand(Base::PhysicalElement<DIM>& el , LinearAlgebra::MiddleSizeVector& ret);
    } elementSpaceIntegrand;

    class anonymous4 : public Integration::ElementIntegrandBase<LinearAlgebra::MiddleSizeVector, DIM>
    {
    public:
        
        void elementIntegrand(Base::PhysicalElement<DIM>& el , LinearAlgebra::MiddleSizeVector& ret);
    } initialConditionsIntegrand;

    class anonymous5 : public Integration::ElementIntegrandBase<LinearAlgebra::MiddleSizeVector, DIM>
    {
    public:
        
        void elementIntegrand(Base::PhysicalElement<DIM>& el , LinearAlgebra::MiddleSizeVector& ret);
    } initialConditionsDerivIntegrand;

    class anonymous6 : public Integration::FaceIntegrandBase<LinearAlgebra::MiddleSizeMatrix, DIM>
    {
    public:
        void faceIntegrand(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeMatrix& ret);
    } faceStiffnessIntegrand;

    class anonymous7 : public Integration::FaceIntegrandBase<LinearAlgebra::MiddleSizeMatrix, DIM>
    {
    public:
        
        void faceIntegrand(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeMatrix& ret);
    } faceStiffnessIntegrandIP;

    class anonymous8 : public Integration::FaceIntegrandBase<LinearAlgebra::MiddleSizeVector, DIM>
    {
    public:
        
        void faceIntegrand(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeVector& ret);
    } faceSpaceIntegrandIP;

    class anonymous9 : public Integration::FaceIntegrandBase<LinearAlgebra::MiddleSizeMatrix, DIM>
    {
    public:
        
        void faceIntegrand(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeMatrix& ret);
    } faceStiffnessIntegrandBR;

    class anonymous10 : public Integration::FaceIntegrandBase<LinearAlgebra::MiddleSizeVector, DIM>
    {
    public:
        
        void faceIntegrand(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeVector& ret);
    } faceSpaceIntegrandBR;

    class anonymous11 : public Integration::FaceIntegrandBase<LinearAlgebra::MiddleSizeVector, DIM>
    {
    public:
        
        void faceIntegrand(Base::PhysicalFace<DIM>& fa, LinearAlgebra::MiddleSizeVector& ret);
    } faceSpaceIntegrand;

    static void initialConditions(const PointPhysicalT& p, LinearAlgebra::SmallVector<DIM>& ret);

    static void initialConditionsDeriv(const PointPhysicalT& p, LinearAlgebra::SmallVector<DIM>& ret);

    static void sourceTerm(const Geometry::PointPhysical<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret);

    static void initialExactSolution(const Geometry::PointPhysical<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret);

    static void boundaryConditions(const Geometry::PointPhysical<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret);
    
};

#endif
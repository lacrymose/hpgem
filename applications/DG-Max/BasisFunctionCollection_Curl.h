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

//this file contains the basis functions and some special vector manipulations that are not needed for more sensible bases
#ifndef BasisFunctionsCollection_Curl_h
#define BasisFunctionsCollection_Curl_h

#include "Base/BaseBasisFunction.h"
#include "Base/MeshManipulator.h"
#include "Base/Element.h"
#include "LinearAlgebra/SmallVector.h"
//should probably be moved to the utilities namespace and another file at some point
/**
 * Computes the 3D outer product of 2 vectors
 * \param [in] a,b The 2 vectors that you want the outer product of.
 * \param [out] ret  a x b
 */

const std::size_t DIM = 3;
void OuterProduct(const LinearAlgebra::SmallVector<DIM>& a, const LinearAlgebra::SmallVector<DIM>& b, LinearAlgebra::SmallVector<DIM>& ret);

namespace Base
{
    /**
     * Computes legendre polynomials of arbitrary degree using a recursive defenition
     * \param [in] degree The degree of the legendre polynomial.
     * \param [in] x The point where you want the function evaluated.
     * \return The legendre polynomial of the requested degree, evaluated at x.
     * \pre -1<=x<=1
     * \pre 0<=degree
     */
    double LegendrePolynomial(int degree, double x);
    
    /**
     * Computes the derivative legendre polynomials of arbitrary degree using a recursive defenition
     * \param [in] degree The degree of the legendre polynomial. This means that the degree of the derivative will be one lower than this number.
     * \param [in] x The point where you want the function evaluated.
     * \return Tthe legendre polynomial of the requested degree, evaluated at x.
     * \pre -1<=x<=1
     * \pre 0<=degree
     */
    double LegendrePolynomialDerivative(int degree, double x);
    
    /**
     * Linear scalar nodal basis functions, with nodes on the element vertices. They are used to give a consistent
     * coordinate system for the elements.
     */
    struct Basis_Curl_Bari : public BaseBasisFunction
    {
        int VertexNr;
        Basis_Curl_Bari(int vertex);

        //pure abstract functions derived from superclass
        virtual double eval(const Geometry::PointReference<DIM>& p) const;
        virtual double evalDeriv0(const Geometry::PointReference<DIM>& p) const;
        virtual double evalDeriv1(const Geometry::PointReference<DIM>& p) const;
        virtual double evalDeriv2(const Geometry::PointReference<DIM>& p) const;

        /**
         * Combines \ref evalDeriv0 , \ref evalDeriv1 and \ref evalDeriv2 into one function.
         * \param [in] p The point where the derivative should be evaluated.
         * \param [out] ret a length 3 vector equal to {\ref evalDeriv0(p),\ref evalDeriv1(p),\ref evalDeriv2(p)}
         */
        virtual LinearAlgebra::SmallVector<DIM> evalDeriv(const Geometry::PointReference<DIM>& p) const;
        static Basis_Curl_Bari barycentricFunctions[];
    };
    
    /**
     * Base class for 3D vector valued functions
     */
    struct threeDBasisFunction : public BaseBasisFunction
    {
        
        //this class has to be derived from basebasisfunctions but we have no use for a scalar eval or the evaluation of a gradient
        /// Function not supported
        virtual double eval(const Geometry::PointReference<DIM>& p) const;
        /// Function not supported
        virtual double evalDeriv0(const Geometry::PointReference<DIM>& p) const;
        /// Function not supported
        virtual double evalDeriv1(const Geometry::PointReference<DIM>& p) const;
        /// Function not supported
        virtual double evalDeriv2(const Geometry::PointReference<DIM>& p) const;

        /**
         * gives a physical point where the function 'is the most active'. A good implementation tries to make sure that
         * integral(||exp(ik*x)phi||^2) is approximately integral(||exp(ik*node)phi||^2) for all k,
         * but usually a decent guess is returned
         */
        virtual void getReasonableNode(const Element& element, Geometry::PointPhysical<DIM> node)=0;

        /**
         * Computes the value of the basis function.
         * \param [in] p The point where the derivative should be evaluated.
         * \param [out] ret The value of the basis function
         */
        virtual void eval(const Geometry::PointReference<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret) const=0;

        /**
         * Computes the curl of the basis functions. Implementations should prefer analytic derivation over numerical.
         * \param [in] p The point where the curl should be evaluated
         * \param [out] ret The curl of the basis function
         */
        virtual LinearAlgebra::SmallVector<DIM> evalCurl(const Geometry::PointReference<DIM>& p) const=0;
    };
    
    //! Curl conforming edge functions.
    struct Basis_Curl_Edge : public threeDBasisFunction
    {
        const int deg, o, i;
        Basis_Curl_Edge(int degree, int localFirstVertex, int localSecondVertex);

        virtual void eval(const Geometry::PointReference<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret) const;

        virtual LinearAlgebra::SmallVector<DIM> evalCurl(const Geometry::PointReference<DIM>& p) const;

        virtual void getReasonableNode(const Element& element, Geometry::PointPhysical<DIM> node);
    };
    
    //! Curl conforming edge based face functions.
    struct Basis_Curl_Edge_Face : public threeDBasisFunction
    {
        int deg, a, b, c;
        Basis_Curl_Edge_Face(int degree, int localOpposingVertex, int localSpecialVertex);

        virtual void eval(const Geometry::PointReference<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret) const;

        virtual LinearAlgebra::SmallVector<DIM> evalCurl(const Geometry::PointReference<DIM>& p) const;

        virtual void getReasonableNode(const Element& element, Geometry::PointPhysical<DIM> node);
    };
    
    //! Curl conforming face functions.
    struct Basis_Curl_Face : public threeDBasisFunction
    {
        int deg1, deg2, a, b, c;
        Basis_Curl_Face(int degree1, int degree2, int localOpposingVertex, int direction);

        virtual void eval(const Geometry::PointReference<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret) const;

        virtual LinearAlgebra::SmallVector<DIM> evalCurl(const Geometry::PointReference<DIM>& p) const;

        virtual void getReasonableNode(const Element& element, Geometry::PointPhysical<DIM> node);
    };
    
    //! Curl conforming face based interior functions.
    struct Basis_Curl_Face_interior : public threeDBasisFunction
    {
        int deg1, deg2, a, b, c, d;
        Basis_Curl_Face_interior(int degree1, int degree2, int localOpposingVertex);

        virtual void eval(const Geometry::PointReference<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret) const;

        virtual LinearAlgebra::SmallVector<DIM> evalCurl(const Geometry::PointReference<DIM>& p) const;

        virtual void getReasonableNode(const Element& element, Geometry::PointPhysical<DIM> node);
    };
    
    //! curl conforming interior functions
    struct Basis_Curl_interior : public threeDBasisFunction
    {
        const int deg1, deg2, deg3, direction;
        Basis_Curl_interior(int degree1, int degree2, int degree3, int direction);

        virtual void eval(const Geometry::PointReference<DIM>& p, LinearAlgebra::SmallVector<DIM>& ret) const;

        virtual LinearAlgebra::SmallVector<DIM> evalCurl(const Geometry::PointReference<DIM>& p) const;

        virtual void getReasonableNode(const Element& element, Geometry::PointPhysical<DIM> node);
    };
}


/**
 * A small extension to allow overriding \ref createBasisFunctions. (Will probably be redundand in the release version of hpGEM2)
 */
class MyMeshManipulator : public Base::MeshManipulator<DIM>
{
    
public:
    MyMeshManipulator(const Base::ConfigurationData* data, Base::BoundaryType xPer = Base::BoundaryType::SOLID_WALL, Base::BoundaryType yPer = Base::BoundaryType::SOLID_WALL, Base::BoundaryType zPer = Base::BoundaryType::SOLID_WALL, std::size_t order = 1, std::size_t idRangeBegin = 0, std::size_t nrOfElementMatrixes = 0, std::size_t nrOfElementVectors = 0, std::size_t nrOfFaceMatrixes = 0, std::size_t nrOfFaceVectors = 0);
    

private:
    void createBasisFunctions(unsigned int order);
};

#endif  // BasisFunctionsCollection_Curl_h
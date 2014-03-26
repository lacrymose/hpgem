/*
 * MappingCubeNLinear.hpp
 *
 *  Created on: Feb 5, 2013
 *      Author: nicorivas
 */

#ifndef MAPPINGHYPERCUBELINEAR_H_
#define MAPPINGHYPERCUBELINEAR_H_

#include "MappingReferenceToPhysical.hpp"
#include "../GlobalNamespaceGeometry.hpp"

namespace Geometry
{
    /*!
     * "In geometry, a hypercube is an n-dimensional analogue of a square (n = 2) and a cube
     * (n = 3)" -Wikipedia.
     *
     * This templated class defines the linear mappings between hypercubes, in the corresponding
     * dimension. See the comments in the Physical<Hypercube>.cpp files to know the order of the
     * vertex of each hypercube, an order which is kept by the mappings.
     */


    template <unsigned int DIM>
    class MappingToPhysHypercubeLinear;

    // ~~~ Dimension 1 ~~~==========================================================================
    template <>
    class MappingToPhysHypercubeLinear<1>: public MappingReferenceToPhysical
    {
        public:
            typedef Geometry::PhysicalGeometry PhysicalGeometryT;
            typedef Geometry::PointReference PointReferenceT;
            typedef Geometry::PointPhysical PointPhysicalT;
            typedef Geometry::Jacobian JacobianT;

        public:
            MappingToPhysHypercubeLinear(const PhysicalGeometryT*const&);
            virtual void transform(const PointReferenceT&, PointPhysicalT&) const;
            virtual void calcJacobian(const PointReferenceT&, JacobianT&) const;
            virtual void reinit(const PhysicalGeometryT* const);
            virtual int getTargetDimension() const {return 1;}

        private:
            bool isValidPoint(const PointReferenceT&) const;
            double mid, slope;
    };

    // ~~~ Dimension 2 ~~~==========================================================================
    template <>
    class MappingToPhysHypercubeLinear<2>: public MappingReferenceToPhysical
    {
        public:
            typedef Geometry::PhysicalGeometry PhysicalGeometryT;
            typedef Geometry::PointReference PointReferenceT;
            typedef Geometry::PointPhysical PointPhysicalT;
            typedef Geometry::Jacobian JacobianT;

        public:
            // Constructor.
            MappingToPhysHypercubeLinear(const PhysicalGeometryT*const&);
            virtual void transform(const PointReferenceT&, PointPhysicalT&) const;
            virtual void calcJacobian(const PointReferenceT&, JacobianT&) const;
            virtual void reinit(const PhysicalGeometryT* const);
            virtual int getTargetDimension() const {return 2;}

        private:
            bool isValidPoint(const PointReferenceT&) const;
            PointPhysicalT a0, a1, a2, a12;
    };

    // ~~~ Dimension 3 ~~~==========================================================================
    template <>
    class MappingToPhysHypercubeLinear<3>: public MappingReferenceToPhysical
    {
        public:
            typedef Geometry::PhysicalGeometry PhysicalGeometryT;
            typedef Geometry::PointReference PointReferenceT;
            typedef Geometry::PointPhysical PointPhysicalT;
            typedef Geometry::Jacobian JacobianT;

        public:
            // Constructor.
            MappingToPhysHypercubeLinear(const PhysicalGeometryT*const&);
            virtual void transform(const PointReferenceT&, PointPhysicalT&) const;
            virtual void calcJacobian(const PointReferenceT&, JacobianT&) const;
            virtual void reinit(const PhysicalGeometryT* const);
            virtual int getTargetDimension() const {return 3;}

        private:
            bool isValidPoint(const PointReferenceT&) const;
            PointPhysicalT a0, a1, a2, a3, a12, a23, a13, a123;
    };

    // ~~~ Dimension 4 ~~~==========================================================================
    template <>
    class MappingToPhysHypercubeLinear<4>: public MappingReferenceToPhysical
    {
    public:
        typedef Geometry::PhysicalGeometry PhysicalGeometryT;
        typedef Geometry::PointReference PointReferenceT;
        typedef Geometry::PointPhysical PointPhysicalT;
        typedef Geometry::Jacobian JacobianT;

    public:
        // Constructor.
        MappingToPhysHypercubeLinear(const PhysicalGeometryT*const&);
        virtual void transform(const PointReferenceT&, PointPhysicalT&) const;
        virtual void calcJacobian(const PointReferenceT&, JacobianT&) const;
        virtual void reinit(const PhysicalGeometryT* const);
        virtual int getTargetDimension() const {return 4;}

    private:
        bool isValidPoint(const PointReferenceT&) const;
        PointPhysicalT abar, a0,   a1,   a2,   a3,
                             a01,  a02,  a03,  a12,  a13,  a23,
                             a012, a013, a123, a230, a0123;
    };
};
#endif

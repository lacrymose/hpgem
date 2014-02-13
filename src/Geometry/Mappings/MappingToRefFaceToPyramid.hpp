/*
 * MappingToRefFaceToPyramid.hpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nicorivas
 */

#ifndef MappingToRefFaceToPyramid_H_
#define MappingToRefFaceToPyramid_H_

#include "MappingReferenceToReference.hpp"
#include "../Jacobian.hpp"

namespace Geometry
{
    /*
     *
     */

    // ~~~ index 0 ~~~==============================================================================

    class MappingToRefFaceToPyramid0: public MappingReferenceToReference
    {
        public:
            static const MappingToRefFaceToPyramid0& Instance();
            virtual void transform(const Geometry::PointReference& p1,
                                         Geometry::PointReference& p2) const;
            virtual void calcJacobian(const Geometry::PointReference&,
                                            Geometry::Jacobian&) const;
            virtual int getTargetDimension() const {return 3;}
        private:
            MappingToRefFaceToPyramid0();
            MappingToRefFaceToPyramid0(const MappingToRefFaceToPyramid0&);
            MappingToRefFaceToPyramid0& operator=(const MappingToRefFaceToPyramid0&);
            virtual ~MappingToRefFaceToPyramid0();
    };

    // ~~~ index 1 ~~~==============================================================================

    class MappingToRefFaceToPyramid1: public MappingReferenceToReference
    {
        public:
            static const MappingToRefFaceToPyramid1& Instance();
            virtual void transform(const Geometry::PointReference&,
                                         Geometry::PointReference&) const;
            virtual void calcJacobian(const Geometry::PointReference&,
                                            Geometry::Jacobian&) const;
            virtual int getTargetDimension() const {return 3;}
        private:
            MappingToRefFaceToPyramid1();
            MappingToRefFaceToPyramid1(const MappingToRefFaceToPyramid1&);
            MappingToRefFaceToPyramid1& operator=(const MappingToRefFaceToPyramid1&);
            virtual ~MappingToRefFaceToPyramid1();
    };

    // ~~~ index 2 ~~~==============================================================================

    class MappingToRefFaceToPyramid2: public MappingReferenceToReference
    {
        public:
            static const MappingToRefFaceToPyramid2& Instance();
            virtual void transform(const Geometry::PointReference& p1,
                                         Geometry::PointReference& p2) const;
            virtual void calcJacobian(const Geometry::PointReference&,
                                            Geometry::Jacobian&) const;
            virtual int getTargetDimension() const {return 3;}
        private:
            MappingToRefFaceToPyramid2();
            MappingToRefFaceToPyramid2(const MappingToRefFaceToPyramid2&);
            MappingToRefFaceToPyramid1& operator=(const MappingToRefFaceToPyramid2&);
            virtual ~MappingToRefFaceToPyramid2();
    };

    // ~~~ index 3 ~~~==============================================================================

    class MappingToRefFaceToPyramid3: public MappingReferenceToReference
    {
        public:
            static const MappingToRefFaceToPyramid3& Instance();
            virtual void transform(const Geometry::PointReference& p1,
                                         Geometry::PointReference& p2) const;
            virtual void calcJacobian(const Geometry::PointReference&,
                                            Geometry::Jacobian&) const;
            virtual int getTargetDimension() const {return 3;}
        private:
            MappingToRefFaceToPyramid3();
            MappingToRefFaceToPyramid3(const MappingToRefFaceToPyramid3&);
            MappingToRefFaceToPyramid3& operator=(const MappingToRefFaceToPyramid3&);
            virtual ~MappingToRefFaceToPyramid3();
    };

    // ~~~ index 4 ~~~==============================================================================

    class MappingToRefFaceToPyramid4: public MappingReferenceToReference
    {
        public:
            static const MappingToRefFaceToPyramid4& Instance();
            virtual void transform(const Geometry::PointReference& p1,
                                         Geometry::PointReference& p2) const;
            virtual void calcJacobian(const Geometry::PointReference&,
                                            Geometry::Jacobian&) const;
            virtual int getTargetDimension() const {return 3;}
        private:
            MappingToRefFaceToPyramid4();
            MappingToRefFaceToPyramid4(const MappingToRefFaceToPyramid4&);
            MappingToRefFaceToPyramid4& operator=(const MappingToRefFaceToPyramid4&);
            virtual ~MappingToRefFaceToPyramid4();
    };

};
#endif

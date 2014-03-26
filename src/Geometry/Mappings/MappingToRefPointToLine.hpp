/*
 * MappingToRefLineToLine.hpp
 *
 *  Created on: Feb 11, 2013
 *      Author: nicorivas
 */

#ifndef MAPPINGPOINTTOLINE_H_
#define MAPPINGPOINTTOLINE_H_

#include "MappingReferenceToReference.hpp"
#include "../Jacobian.hpp"

namespace Geometry
{
    /*
     * The reference line:
     *
     * (-1) 0-------1 (+1)
     *
     * Linear map a point into a line. There are only two possible mappings:
     *
     *      index 0: () -> -1.0
     *      index 1: () -> 1.0
     *
     * \todo I don't quite get this.
     *
     */

    // ~~~ index 0 ~~~=========================================================================== //
    class MappingToRefPointToLine0: public MappingReferenceToReference
    {
        public:
            static const MappingToRefPointToLine0& Instance();
            virtual void transform(const Geometry::PointReference&,
                                         Geometry::PointReference& p2) const;
            virtual void calcJacobian(const Geometry::PointReference&,
                                            Geometry::Jacobian&) const;
            virtual int getTargetDimension() const {return 1;}
        private:
            MappingToRefPointToLine0();
            MappingToRefPointToLine0(const MappingToRefPointToLine0&);
            MappingToRefPointToLine0& operator=(const MappingToRefPointToLine0&);
            virtual ~MappingToRefPointToLine0();
    };

    // ~~~ index 1 ~~~=========================================================================== //
    class MappingToRefPointToLine1: public MappingReferenceToReference
    {
        public:
            static const MappingToRefPointToLine1& Instance();
            virtual void transform(const Geometry::PointReference& p1,
                                         Geometry::PointReference& p2) const;
            virtual void calcJacobian(const Geometry::PointReference&,
                                            Geometry::Jacobian&) const;
            virtual int getTargetDimension() const {return 1;}
        private:
            MappingToRefPointToLine1();
            MappingToRefPointToLine1(const MappingToRefPointToLine1&);
            MappingToRefPointToLine1& operator=(const MappingToRefPointToLine1&);
            virtual ~MappingToRefPointToLine1();
    };
};
#endif

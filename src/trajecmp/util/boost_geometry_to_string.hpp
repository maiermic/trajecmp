#ifndef TRAJECMP_UTIL_BOOST_GEOMETRY_TO_STRING_H
#define TRAJECMP_UTIL_BOOST_GEOMETRY_TO_STRING_H

#include <ostream>
#include <boost/geometry.hpp>

namespace boost { namespace geometry { namespace model {

    template<typename T, std::size_t S, typename CS>
    std::ostream &operator<<(std::ostream &os, point <T, S, CS> const &value) {
        os << boost::geometry::wkt(value);
        return os;
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &os, linestring <T> const &value) {
        os << boost::geometry::wkt(value);
        return os;
    }

    template<typename T>
    bool operator==(const linestring <T> &lhs, const linestring <T> &rhs) {
        namespace bg = boost::geometry;
        // fix bug of boost::geometry::equals
        // see https://svn.boost.org/trac/boost/ticket/12929
        if (bg::is_empty(lhs)) {
            return bg::is_empty(rhs);
        } else if (1 == bg::num_points(lhs) && 1 == bg::num_points(rhs)) {
            return bg::equals(lhs[0], rhs[0]);
        }
        return bg::equals(lhs, rhs);
    }

    template
            <
                    typename CoordinateType,
                    std::size_t DimensionCount,
                    typename CoordinateSystem
            >
    bool operator==(const point <CoordinateType, DimensionCount, CoordinateSystem> &lhs,
                    const point <CoordinateType, DimensionCount, CoordinateSystem> &rhs) {
        return boost::geometry::equals(lhs, rhs);
    }

}}} // namespace boost::geometry::model

#endif //TRAJECMP_UTIL_BOOST_GEOMETRY_TO_STRING_H

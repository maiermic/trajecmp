#ifndef TRAJECTORY_DETECTION_BOOST_GEOMETRY_TO_STRING_H
#define TRAJECTORY_DETECTION_BOOST_GEOMETRY_TO_STRING_H

#include <iostream>
#include <boost/geometry.hpp>

namespace boost {
    namespace geometry {
        namespace model {
            template<typename T, std::size_t S, typename CS>
            std::ostream &operator<<(std::ostream &os, point<T, S, CS> const &value) {
                os << ::boost::geometry::wkt(value);
                return os;
            }

            template<typename T>
            std::ostream &operator<<(std::ostream &os, linestring<T> const &value) {
                os << ::boost::geometry::wkt(value);
                return os;
            }

            template<typename T>
            bool operator==(const linestring<T> &lhs, const linestring<T> &rhs) {
                return ::boost::geometry::equals(lhs, rhs);
            }
        }
    }
}

#endif //TRAJECTORY_DETECTION_BOOST_GEOMETRY_TO_STRING_H

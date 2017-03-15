#include <catch.hpp>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;

namespace boost {
    namespace geometry {
        namespace model {
            template <typename T, std::size_t S, typename CS>
            std::ostream& operator << ( std::ostream& os, point<T, S, CS> const& value ) {
                os << bg::wkt( value );
                return os;
            }
            template <typename T>
            std::ostream& operator << ( std::ostream& os, linestring<T> const& value ) {
                os << bg::wkt( value );
                return os;
            }
            template <typename T>
            bool operator ==(const linestring<T> & lhs, const linestring<T> & rhs) {
                return ::bg::equals(lhs, rhs);
            }
        }
    }
}

TEST_CASE("point example", "[BoostGeometry]") {
    bg::model::point<double, 2, bg::cs::cartesian> point1;
    bg::model::point<double, 3, bg::cs::cartesian> point2(1.0, 2.0, 3.0);

    bg::set<0>(point1, 1.0);
    point1.set<1>(2.0);

    double x = bg::get<0>(point1);
    double y = point1.get<1>();

    CAPTURE(point1);
    CAPTURE(point2);
    CHECK(1.0 == x);
    CHECK(2.0 == y);
}

TEST_CASE("linestring example", "[BoostGeometry]") {
    typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
    typedef bg::model::linestring<point_t> linestring_t;

    linestring_t ls1;
    bg::append(ls1, point_t(0.0, 0.0));
    bg::append(ls1, point_t(1.0, 0.0));
    bg::append(ls1, point_t(1.0, 2.0));

//    linestring_t ls2{{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};
    linestring_t ls2;

    CHECK(ls1 == ls2);
}

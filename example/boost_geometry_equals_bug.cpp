#include <iostream>

#include <boost/geometry.hpp>

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;

int main() {
    // fix bug of boost::geometry::equals
    // see https://svn.boost.org/trac/boost/ticket/12929
    linestring empty;
    linestring single_point_linestring {{0.0, 0.0}};
    linestring single_point_linestring_2 {{0.0, 0.0}};

    std::cout << "empty == empty := " << bg::equals(empty, empty) << '\n';
    std::cout << "single_point_linestring == single_point_linestring := "
              << bg::equals(single_point_linestring, single_point_linestring) << '\n';
    std::cout << "single_point_linestring == single_point_linestring_2 := "
              << bg::equals(single_point_linestring, single_point_linestring_2) << '\n';
}
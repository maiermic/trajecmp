#include <iostream>
#include <string>

#include <boost/geometry.hpp>

#include "../test/boost_geometry_to_string.h"
#include "coordinate_iterator.h"
#include "Miniball.hpp"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;

#define LOG(value) \
    std::cout << #value << ": " << value << '\n';

int main() {
    point p{7.0, 3.0};
    LOG(p);

    LOG(get(0, p));
    LOG(get(1, p));

    CoordinateIterator<point> it(p, 0);
    double first = *it;
    LOG(first);
    ++it;
    double second = *it;
    LOG(second);
//    LOG(*(++it));
//    LOG(*(++it));
}
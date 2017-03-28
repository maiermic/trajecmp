#include <iostream>
#include <string>

#include <boost/geometry.hpp>

#include "../src/boost_geometry_to_string.h"
#include "../src/coordinate_iterator.h"
#include "../src/Miniball.hpp"

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
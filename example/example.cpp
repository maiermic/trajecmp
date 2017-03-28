#include <iostream>

#include <boost/geometry.hpp>
#include "../src/distance.h"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;
using Trajectory = linestring;

int main() {
    const Trajectory input_trajectory{{0, 2},
                                      {0, 0},
                                      {1, 0}};

    // define pattern trajectory
    const Trajectory pattern_letter_L{{0, 2},
                                      {0, 0},
                                      {1, 0}};

    // --------------
    // preprocessing
    // --------------



    // configure distance measurement
    distance::neighbours_percentage_range neighbours(0.1);

    // calculate distance
    const auto distance = distance::modified_hausdorff(input_trajectory,
                                                       pattern_letter_L,
                                                       neighbours);

    // decide similarity based on distance
    const auto is_similar = (0 == distance);


}
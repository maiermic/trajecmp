#include <iostream>

#include <boost/geometry.hpp>
#include "../src/distance.h"
#include "../src/boost_geometry_to_string.h"
#include "../src/logging.h"
#include "../src/MinBoundingSphere.h"

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;
using Trajectory = linestring;

int main() {
    namespace bg = boost::geometry;
    namespace trans = boost::geometry::strategy::transform;

    logging::is_logging = true;

    const Trajectory input_trajectory{{10, 2},
                                      {10, 0},
                                      {11, 0}};

    // define pattern trajectory
    const Trajectory pattern_letter_L{{0, 2},
                                      {0, 0},
                                      {1, 0}};

    // --------------
    // preprocessing
    // --------------

    MinBoundingSphere<Trajectory> input_mbs(input_trajectory);
    MinBoundingSphere<Trajectory> pattern_mbs(pattern_letter_L);

    // translate
    point translation_vector(input_mbs.center);
    bg::subtract_point(translation_vector, pattern_mbs.center);

    Trajectory translated_trajectory;
    trans::translate_transformer<double, 2, 2> translate(bg::get<0>(translation_vector), bg::get<1>(translation_vector));
    boost::geometry::transform(pattern_letter_L, translated_trajectory, translate);
    LOG(translated_trajectory);

    // scale

    // rotate
    // no


    // ---------------------
    // compare trajectories
    // ---------------------

    // configure distance measurement
    distance::neighbours_percentage_range neighbours(0.1);

    // calculate distance
    const auto distance = distance::modified_hausdorff(input_trajectory,
                                                       translated_trajectory,
                                                       neighbours);

    // decide similarity based on distance
    const auto is_similar = (0 == distance);

    LOG(is_similar);
}
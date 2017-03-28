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

    const Trajectory input_trajectory{{10, 4},
                                      {10, 0},
                                      {12, 0}};

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
    point pattern_translation_vector(pattern_mbs.center);
    bg::multiply_value(pattern_translation_vector, -1);
    LOG(pattern_translation_vector);

    Trajectory pattern_translated;
    trans::translate_transformer<double, 2, 2> pattern_translate(bg::get<0>(pattern_translation_vector),
                                                         bg::get<1>(pattern_translation_vector));
    boost::geometry::transform(pattern_letter_L, pattern_translated, pattern_translate);
    LOG(pattern_translated);


    point input_translation_vector(input_mbs.center);
    bg::multiply_value(input_translation_vector, -1);
    LOG(input_translation_vector);

    Trajectory input_translated;
    trans::translate_transformer<double, 2, 2> input_translate(bg::get<0>(input_translation_vector),
                                                         bg::get<1>(input_translation_vector));
    boost::geometry::transform(input_trajectory, input_translated, input_translate);
    LOG(input_translated);


    // scale
    const double normalized_size = 10000;

    Trajectory pattern_scaled;
    trans::scale_transformer<double, 2, 2> pattern_scale(normalized_size / pattern_mbs.radius);
    boost::geometry::transform(pattern_translated, pattern_scaled, pattern_scale);
    LOG(pattern_scaled);

    Trajectory input_scaled;
    trans::scale_transformer<double, 2, 2> input_scale(normalized_size / input_mbs.radius);
    boost::geometry::transform(input_translated, input_scaled, input_scale);
    LOG(input_scaled);


    // rotate
    // no


    const Trajectory &transformed_input = input_scaled;
    const Trajectory &transformed_pattern = pattern_scaled;

    // ---------------------
    // compare trajectories
    // ---------------------

    // configure distance measurement
    distance::neighbours_percentage_range neighbours(0.1);

    // calculate distance
    const auto distance = distance::modified_hausdorff(transformed_input,
                                                       transformed_pattern,
                                                       neighbours);

    // decide similarity based on distance
    const auto is_similar = (0 == distance);

    LOG(is_similar);
}
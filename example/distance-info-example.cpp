#include <iostream>
#include <algorithm>

#include <boost/geometry.hpp>
#include <boost/geometry/extensions/algorithms/distance_info.hpp>

#include "trajecmp/functional/functional.hpp"
#include "trajecmp/distance/modified_hausdorff.hpp"
#include "trajecmp/geometry/min_bounding_sphere.hpp"
#include "trajecmp/geometry/vector.hpp"
#include "trajecmp/transform/translate.hpp"
#include "trajecmp/transform/scale.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"


#include "logging.hpp"


int main() {
    namespace bg = boost::geometry;
    using point = bg::model::point<double, 2, bg::cs::cartesian>;
    using linestring = bg::model::linestring<point>;
    using distance_info_result = bg::distance_info_result<point>;

    point p {-4, 1};
    linestring l {
            {-3, 2},
            {1, 2},
            {1, 4},
            {10, 4},
    };
    distance_info_result result;
    bg::distance_info(p, l, result);

    logging::is_logging = true;
    LOG(*std::begin(l));
    LOG(result.on_segment);
    LOG(result.within_geometry);
    LOG(result.real_distance);
    LOG(result.projected_point1);
    LOG(result.projected_point2);
    LOG(result.projected_distance1);
    LOG(result.projected_distance2);
    LOG(result.fraction1);
    LOG(result.fraction2);
//    LOG(result.seg_id1);
//    LOG(result.seg_id2);
}
#include <vector>
#include <iostream>
#include <functional>

#include <boost/geometry.hpp>

#include "logging.hpp"
#include "../src/trajecmp/functional/functional.hpp"
#include "../src/trajecmp/distance/modified_hausdorff.hpp"
#include "../src/trajecmp/geometry/min_bounding_sphere.hpp"
#include "../src/trajecmp/geometry/vector.hpp"
#include "../src/trajecmp/transform/translate.hpp"
#include "../src/trajecmp/transform/scale.hpp"

#include "../src/trajecmp/util/boost_geometry_to_string.hpp"
#include "TrajectorySvg.hpp"

#include "rxcpp/rx.hpp"

namespace Rx {
    using namespace rxcpp;
    using namespace rxcpp::sources;
    using namespace rxcpp::operators;
    using namespace rxcpp::util;
}

using point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using linestring = boost::geometry::model::linestring<point>;
using Trajectory = linestring;
using vector = point;

auto has_min_num_points(int min) {
    return [=](const auto &trajectory) {
        return boost::geometry::num_points(trajectory) >= min;
    };
}

auto less_than(int upper) {
    return [=](int value) {
        return value <= upper;
    };
}

Rx::rxsub::subject<Trajectory> input_trajectory_subject;

int main() {
    logging::is_logging = true;

    auto input_trajectory_stream = input_trajectory_subject.get_observable();
    auto pattern_trajectory_stream =
            Rx::observable<>::just(
                    Trajectory{
                            {0, 2},
                            {0, 0},
                            {1, 0}
                    }
            );
    const auto transform = [](Trajectory &trajectory) {
        const auto mbs = trajecmp::geometry::min_bounding_sphere(trajectory);
        return trajecmp::transform::scale_to_const<100>(mbs.radius)(
                trajecmp::transform::translate_by(trajecmp::geometry::negative_vector_of(mbs.center))(trajectory)
        );
    };

    auto preprocessed_input_trajectory_stream =
            input_trajectory_stream
                    .filter(has_min_num_points(2))
                    .map(transform);
    auto preprocessed_pattern_trajectory_stream =
            pattern_trajectory_stream
                    .filter(has_min_num_points(2))
                    .map(transform);
    const trajecmp::distance::neighbours_percentage_range neighbours(0.1);
    const auto modified_hausdorff = trajecmp::distance::modified_hausdorff(neighbours);
    auto distance_stream =
            preprocessed_input_trajectory_stream
                    .with_latest_from(modified_hausdorff, preprocessed_pattern_trajectory_stream);
    auto input_matches_pattern_stream = distance_stream.filter(less_than(25));
    input_matches_pattern_stream
            .subscribe([](double distance) {
                std::cout << "input matches pattern with distance " << distance << '\n';
            });


    auto subscriber = input_trajectory_subject.get_subscriber();
    subscriber.on_next(Trajectory {
            {128, 405},
            {130, 397},
            {130, 381},
            {125, 331},
            {123, 275},
            {117, 235},
            {120, 233},
            {161, 238},
            {185, 238},
            {192, 232},
            {199, 230},
            {197, 232},
    });
    subscriber.on_completed();
}
#include <vector>
#include <iostream>
#include <functional>

#include <boost/geometry.hpp>
#include <rxcpp/rx.hpp>

#include "logging.hpp"
#include "trajecmp/functional/functional.hpp"
#include "trajecmp/distance/modified_hausdorff.hpp"
#include "trajecmp/geometry/min_bounding_sphere.hpp"
#include "trajecmp/geometry/vector.hpp"
#include "trajecmp/transform/translate.hpp"
#include "trajecmp/transform/scale.hpp"
#include "trajecmp/predicate/has_min_num_points.hpp"
#include "trajecmp/compare/less_than.hpp"
#include "trajecmp/compare/match_by.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "TrajectorySvg.hpp"


using point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using linestring = boost::geometry::model::linestring<point>;
using Trajectory = linestring;
using vector = point;


rxcpp::rxsub::subject<Trajectory> input_trajectory_subject;
const auto input_trajectory_stream = input_trajectory_subject.get_observable();

int main() {
    using trajecmp::predicate::has_min_num_points;
    using trajecmp::compare::less_than;
    using trajecmp::compare::match_by;
    using trajecmp::util::subscribe_with_latest_from;

    logging::is_logging = true;

    auto pattern_L_trajectory_stream =
            rxcpp::observable<>::just(
                    Trajectory{
                            {0, 2},
                            {0, 0},
                            {1, 0},
                    }
            );
    auto pattern_M_trajectory_stream =
            rxcpp::observable<>::just(
                    Trajectory{
                            {0, 0},
                            {1, 4},
                            {2, 0},
                            {3, 4},
                            {4, 0},
                    }
            );

    static const auto normalized_size = 100;
    const auto transform = [=](Trajectory &trajectory) {
        const auto mbs = trajecmp::geometry::min_bounding_sphere(trajectory);
        return trajecmp::transform::scale_to_const<normalized_size>(mbs.radius * 2)(
                trajecmp::transform::translate_by(trajecmp::geometry::negative_vector_of(mbs.center))(trajectory)
        );
    };

    const auto preprocess = [&transform](auto &&trajectory_stream) {
        return trajectory_stream
                .filter(has_min_num_points(2))
                .map(transform);
    };
    auto preprocessed_input_trajectory_stream = preprocess(input_trajectory_stream);
    auto preprocessed_pattern_L_trajectory_stream = preprocess(pattern_L_trajectory_stream);
    auto preprocessed_pattern_M_trajectory_stream = preprocess(pattern_M_trajectory_stream);
    const trajecmp::distance::neighbours_percentage_range neighbours(0.1);
    const auto modified_hausdorff = trajecmp::distance::modified_hausdorff(neighbours);
    const auto compare = match_by(modified_hausdorff, less_than(normalized_size * 0.25));
    auto input_matches_pattern_L_stream =
            compare(preprocessed_input_trajectory_stream,
                    preprocessed_pattern_L_trajectory_stream);
    auto input_matches_pattern_M_stream =
            compare(preprocessed_input_trajectory_stream,
                    preprocessed_pattern_M_trajectory_stream);


    const auto visualization_size = 500;
    const auto transform_for_visualization = trajecmp::functional::pipe(
            trajecmp::transform::scale_to_const<visualization_size>(normalized_size),
            trajecmp::transform::translate_by(vector(visualization_size / 2, visualization_size / 2))
    );

    input_matches_pattern_L_stream
            | subscribe_with_latest_from(
                    [&](auto distance, auto &&input_trajcetory, auto &&pattern_trajcetory) {
                        std::cout << "transformed input trajectory that matches pattern L with distance of "
                                  << distance << ": " << input_trajcetory << '\n';
                        TrajectorySvg svg("pattern-L.svg", visualization_size, visualization_size);
                        svg.add(transform_for_visualization(input_trajcetory), "visualization_normalized_input");
                        svg.add(transform_for_visualization(pattern_trajcetory), "visualization_normalized_pattern");
                    },
                    preprocessed_input_trajectory_stream,
                    preprocessed_pattern_L_trajectory_stream
            );
    input_matches_pattern_M_stream
            | subscribe_with_latest_from(
                [&](auto distance, auto &&input_trajcetory, auto &&pattern_trajcetory) {
                    std::cout << "transformed input trajectory that matches pattern M with distance of "
                              << distance << ": " << input_trajcetory << '\n';
                    TrajectorySvg svg("pattern-M.svg", visualization_size, visualization_size);
                    svg.add(transform_for_visualization(input_trajcetory), "visualization_normalized_input");
                    svg.add(transform_for_visualization(pattern_trajcetory), "visualization_normalized_pattern");
                },
                preprocessed_input_trajectory_stream,
                preprocessed_pattern_M_trajectory_stream
            );


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
    subscriber.on_next(Trajectory{
            {0, 0},
            {1, 4},
            {2, 0},
            {3, 4},
            {4, 0},
    });
    subscriber.on_completed();
}
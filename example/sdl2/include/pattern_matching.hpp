#ifndef PATTERN_MATCHING_HPP
#define PATTERN_MATCHING_HPP

#include <rxcpp/rx.hpp>

#include "../../../src/trajecmp/functional/functional.hpp"
#include "../../../src/trajecmp/distance/modified_hausdorff.hpp"
#include "../../../src/trajecmp/geometry/min_bounding_sphere.hpp"
#include "../../../src/trajecmp/geometry/vector.hpp"
#include "../../../src/trajecmp/transform/translate.hpp"
#include "../../../src/trajecmp/transform/scale.hpp"
#include "../../../src/trajecmp/transform/douglas_peucker.hpp"
#include "../../../src/trajecmp/predicate/has_min_num_points.hpp"
#include "../../../src/trajecmp/compare/less_than.hpp"
#include "../../../src/trajecmp/compare/match_by.hpp"

#include "debug.hpp"
#include "model.hpp"
#include "pattern.hpp"


namespace pattern_matching {
    using rxcpp::operators::tap;
    using rxcpp::operators::publish;
    using rxcpp::operators::ref_count;
    using trajecmp::compare::less_than;
    using trajecmp::compare::match_by;
    using trajecmp::transform::douglas_peucker;

    const rxcpp::rxsub::subject<model::trajectory> input_trajectory_subject;
    const auto input_trajectory_stream = input_trajectory_subject.get_observable();

    const rxcpp::rxsub::subject<model::trajectory> pattern_trajectory_subject;
    const auto pattern_trajectory_stream = pattern_trajectory_subject.get_observable();

    static constexpr auto normalized_size = 100;

    inline auto preprocess(rxcpp::observable<model::trajectory> trajectory_stream,
                    std::string trajectory_name) {
        return trajectory_stream
                .filter([=](auto trajectory) {
                    static const auto min_length = 2;
                    const auto has_min_num_points =
                            trajecmp::predicate::has_min_num_points(min_length)(trajectory);
                    if (!has_min_num_points) {
                        std::cout << trajectory_name
                                  << " needs at least "
                                  << min_length
                                  << " points, but has "
                                  << boost::geometry::num_points(trajectory)
                                  << " points\n";
                    }
                    return has_min_num_points;
                })
                .publish().ref_count()
                .map([](model::trajectory trajectory) {
                    const auto mbs = trajecmp::geometry::min_bounding_sphere(trajectory);
                    return trajecmp::transform::scale_to_const<normalized_size>(mbs.radius * 2)(
                            trajecmp::transform::translate_by(trajecmp::geometry::negative_vector_of(mbs.center))(
                                    trajectory)
                    );
                });
    };

    const auto preprocessed_input_trajectory_stream =
            preprocess(input_trajectory_stream.map(douglas_peucker(3)), "input trajectory");
    const auto preprocessed_pattern_trajectory_stream = preprocess(pattern_trajectory_stream, "pattern trajectory");
    const trajecmp::distance::neighbours_percentage_range neighbours(0.1);
    const auto modified_hausdorff = trajecmp::distance::modified_hausdorff(neighbours);
    const auto modified_hausdorff_info = trajecmp::distance::modified_hausdorff_info(neighbours);
    const auto compare = match_by(modified_hausdorff, less_than(normalized_size * 0.20));

    const auto distance_stream =
            preprocessed_input_trajectory_stream
                    .with_latest_from(modified_hausdorff_info, preprocessed_pattern_trajectory_stream);

} // namespace pattern_matching


#endif //PATTERN_MATCHING_HPP

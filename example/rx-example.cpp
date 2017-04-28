#include <vector>
#include <iostream>
#include <functional>

#include <boost/geometry.hpp>
#include <rxcpp/rx.hpp>

#include "logging.hpp"
#include "../src/trajecmp/functional/functional.hpp"
#include "../src/trajecmp/distance/modified_hausdorff.hpp"
#include "../src/trajecmp/geometry/min_bounding_sphere.hpp"
#include "../src/trajecmp/geometry/vector.hpp"
#include "../src/trajecmp/transform/translate.hpp"
#include "../src/trajecmp/transform/scale.hpp"

#include "../src/trajecmp/util/boost_geometry_to_string.hpp"
#include "TrajectorySvg.hpp"


using point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using linestring = boost::geometry::model::linestring<point>;
using Trajectory = linestring;
using vector = point;

auto has_min_num_points(std::size_t min) {
    return [=](const auto &trajectory) {
        return boost::geometry::num_points(trajectory) >= min;
    };
}

template<typename T>
auto less_than(T upper) {
    return [=](T value) {
        return value <= upper;
    };
}

template<typename D, typename P>
auto match_by(const D &distance_of, const P &predicate) {
    return [=](const auto &input_trajectory_stream, const auto &pattern_trajectory_stream) {
        return input_trajectory_stream
                .with_latest_from(distance_of, pattern_trajectory_stream)
                .filter(predicate);
    };
};

template<typename Fn, typename... Observables>
auto subscribe_with_latest_from(Fn f, Observables... observables) {
    return [=](auto &&source) {
        return source
                .with_latest_from(
                        [=](auto &&...args) {
                            f(args...);
                            return 0; // dummy value
                        },
                        observables...
                )
                .subscribe([](auto _) {});
    };
}


rxcpp::rxsub::subject<Trajectory> input_trajectory_subject;

int main() {
    logging::is_logging = true;

    auto input_trajectory_stream = input_trajectory_subject.get_observable();
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
        return trajecmp::transform::scale_to_const<normalized_size>(mbs.radius)(
                trajecmp::transform::translate_by(trajecmp::geometry::negative_vector_of(mbs.center))(trajectory)
        );
    };

    const auto preprocess = [&transform](auto &&trajectory_stream) {
        return trajectory_stream
                .filter(has_min_num_points(2))
                .map(transform);
    };
    auto preprocessed_input_trajectory_stream =
            preprocess(input_trajectory_stream);
    const trajecmp::distance::neighbours_percentage_range neighbours(0.1);
    const auto modified_hausdorff = trajecmp::distance::modified_hausdorff(neighbours);
    const auto compare = match_by(modified_hausdorff, less_than(normalized_size * 0.25));
    auto input_matches_pattern_L_stream =
            compare(preprocessed_input_trajectory_stream,
                    preprocess(pattern_L_trajectory_stream));
    input_matches_pattern_L_stream
            | subscribe_with_latest_from(
                    [](auto distance, auto &&input_trajcetory) {
                        std::cout << "transformed input trajectory that matches pattern L with distance of "
                                  << distance << ": " << input_trajcetory << '\n';
                    },
                    preprocessed_input_trajectory_stream
            );
    auto input_matches_pattern_M_stream =
            compare(preprocessed_input_trajectory_stream,
                    preprocess(pattern_M_trajectory_stream));
    input_matches_pattern_M_stream
            | subscribe_with_latest_from(
                [](auto distance, auto &&input_trajcetory) {
                    std::cout << "transformed input trajectory that matches pattern M with distance of "
                              << distance << ": " << input_trajcetory << '\n';
                },
                preprocessed_input_trajectory_stream
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
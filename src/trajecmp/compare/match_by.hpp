#ifndef TRAJECMP_COMPARE_MATCH_BY_HPP
#define TRAJECMP_COMPARE_MATCH_BY_HPP


namespace trajecmp { namespace compare {

    template<typename D, typename P>
    auto match_by(const D &distance_of, const P &predicate) {
        return [=](const auto &input_trajectory_stream, const auto &pattern_trajectory_stream) {
            return input_trajectory_stream
                    .with_latest_from(distance_of, pattern_trajectory_stream)
                    .filter(predicate);
        };
    };

}} // namespace trajecmp::compare


#endif //TRAJECMP_COMPARE_MATCH_BY_HPP

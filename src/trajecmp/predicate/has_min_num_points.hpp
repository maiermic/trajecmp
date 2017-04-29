#ifndef TRAJECMP_PREDICATE_HAS_MIN_NUM_POINTS_HPP
#define TRAJECMP_PREDICATE_HAS_MIN_NUM_POINTS_HPP


namespace trajecmp { namespace predicate {

    auto has_min_num_points(std::size_t min) {
        return [=](const auto &trajectory) {
            return boost::geometry::num_points(trajectory) >= min;
        };
    }

}} // namespace trajecmp::predicate


#endif //TRAJECMP_PREDICATE_HAS_MIN_NUM_POINTS_HPP

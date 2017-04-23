#ifndef TRAJECMP_TRANSFORM_SCALE_HPP
#define TRAJECMP_TRANSFORM_SCALE_HPP

namespace trajecmp { namespace transform {

    template<std::size_t ScaledSize, typename CalculationType>
    auto scale_to_const(const CalculationType current_size) {
        return [=](auto &&trajectory) {
            using Trajectory = typename std::remove_reference<decltype(trajectory)>::type;
            static const auto dimension = boost::geometry::dimension<Trajectory>::value;
            namespace trans = boost::geometry::strategy::transform;
            using scale_transformer = trans::scale_transformer<CalculationType, dimension, dimension>;
            Trajectory scaled;
            boost::geometry::transform(trajectory,
                                       scaled,
                                       scale_transformer(ScaledSize / current_size));
            return scaled;
        };
    }

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_SCALE_HPP

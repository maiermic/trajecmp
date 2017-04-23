#ifndef TRAJECMP_TRANSFORM_TRANSLATE_HPP
#define TRAJECMP_TRANSFORM_TRANSLATE_HPP

namespace trajecmp { namespace transform {

    namespace detail {

        namespace trans = boost::geometry::strategy::transform;

        template<typename Vector, typename CalculationType, std::size_t Dimension>
        class vector_translate_transformer {
        };

        template<typename Vector, typename CalculationType>
        struct vector_translate_transformer<Vector, CalculationType, 2>
                : public trans::translate_transformer<CalculationType, 2, 2> {

            vector_translate_transformer(const Vector &translation_vector)
                    : trans::translate_transformer<CalculationType, 2, 2>(boost::geometry::get<0>(translation_vector),
                                                                          boost::geometry::get<1>(translation_vector)) {
            };
        };

        template<typename Vector, typename CalculationType>
        struct vector_translate_transformer<Vector, CalculationType, 3>
                : public trans::translate_transformer<CalculationType, 3, 3> {

            vector_translate_transformer(const Vector &translation_vector)
                    : trans::translate_transformer<CalculationType, 3, 3>(boost::geometry::get<0>(translation_vector),
                                                                          boost::geometry::get<1>(translation_vector),
                                                                          boost::geometry::get<2>(translation_vector)) {
            };
        };

        template<
                typename Vector,
                typename CalculationType = typename boost::geometry::coordinate_type<Vector>::type,
                std::size_t Dimension = boost::geometry::dimension<Vector>::value
        >
        using simple_vector_translate_transformer = vector_translate_transformer<Vector, CalculationType, Dimension>;

    } // namespace detail

    template<typename Vector>
    auto translate_by(const Vector &translation_vector) {
        return [=](auto &&trajectory) {
            using Trajectory = typename std::remove_reference<decltype(trajectory)>::type;
            Trajectory translated;
            boost::geometry::transform(trajectory,
                                       translated,
                                       detail::simple_vector_translate_transformer<Vector>(translation_vector));
            return translated;
        };
    }

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_TRANSLATE_HPP

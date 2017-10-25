#ifndef TRAJECMP_TRANSFORM_SCALE_HPP
#define TRAJECMP_TRANSFORM_SCALE_HPP

#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/for_each.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

#include <trajecmp/util/type_traits.hpp>

namespace trajecmp { namespace transform {

    using boost::geometry::degree;
    using boost::geometry::radian;

    /**
     * Create functor that rotates a trajectory about origin.
     *
     * @tparam DegreeOrRadian Type of rotation angle specification (degree or
     * radian).
     * @tparam Angle Type of angle has to be compatible with the coordinate type
     * of the trajectory (points) that are transformed.
     * @param angle The angle of the rotation.
     * @return Functor that takes a trajectory and returns a rotated copy of it.
     */
    template<
            typename DegreeOrRadian = boost::geometry::degree,
            typename Angle = void
    >
    auto rotate(const Angle angle) {
        return [=](auto &&trajectory) {
            namespace bg = boost::geometry;
            using Trajectory = typename trajecmp::util::remove_cv_ref<decltype(trajectory)>::type;
            using Point = typename bg::point_type<Trajectory>::type;
            using CoordinateType = typename bg::coordinate_type<Trajectory>::type;
            static const auto dimension = bg::dimension<Trajectory>::value;
            namespace trans = bg::strategy::transform;
            using rotate_transformer = trans::rotate_transformer<DegreeOrRadian, CoordinateType, 2, 2>;
            Trajectory rotated;
            bg::for_each_point(
                    trajectory,
                    [angle, &rotated](const Point &point) {
                        Point rotated_point;
                        bg::transform(point,
                                      rotated_point,
                                      rotate_transformer(angle));
                        bg::append(rotated, rotated_point);
                    }
            );
            return rotated;
        };
    }

}}

#endif //TRAJECMP_TRANSFORM_SCALE_HPP

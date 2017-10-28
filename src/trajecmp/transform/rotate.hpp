#ifndef TRAJECMP_TRANSFORM_SCALE_HPP
#define TRAJECMP_TRANSFORM_SCALE_HPP

#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/for_each.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

#include <boost/qvm/quat_vec_operations.hpp>

#include <trajecmp/trait/type_qualifiers.hpp>

namespace trajecmp { namespace transform {

    using boost::geometry::degree;
    using boost::geometry::radian;

    /**
     * Create functor that rotates a 2D trajectory about origin.
     *
     * @tparam DegreeOrRadian Type of rotation angle specification (degree or
     * radian).
     * @tparam Angle Type of angle has to be compatible with the coordinate type
     * of the trajectory (points) that are transformed.
     * @param angle The angle of the rotation.
     * @return Functor that takes a 2D trajectory and returns a rotated copy of
     * it.
     */
    template<
            typename DegreeOrRadian = boost::geometry::degree,
            typename Angle = void
    >
    auto rotate(const Angle angle) {
        return [=](auto &&trajectory) {
            namespace bg = boost::geometry;
            using Trajectory = typename trajecmp::trait::remove_cv_ref<decltype(trajectory)>::type;
            using Point = typename bg::point_type<Trajectory>::type;
            using CoordinateType = typename bg::coordinate_type<Trajectory>::type;
            static const auto dimension = bg::dimension<Trajectory>::value;
            static_assert(dimension == 2,
                          "angle rotation is only supported for 2D trajectories");
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

    /**
     * Create functor that rotates a 3D trajectory using a quaternion.
     *
     * @tparam Quaternion Type that fullfills the quaternion concept.
     * @param quaternion The quaternion used for the rotation.
     * @return Functor that takes a 3D trajectory and returns a rotated copy of
     * it.
     */
    template<typename Quaternion>
    auto rotate_using_quaternion(const Quaternion &quaternion) {
        return [=](auto &&trajectory) {
            namespace bg = boost::geometry;
            using Trajectory = typename trajecmp::trait::remove_cv_ref<decltype(trajectory)>::type;
            using Point = typename bg::point_type<Trajectory>::type;
            using CoordinateType = typename bg::coordinate_type<Trajectory>::type;
            static const auto dimension = bg::dimension<Trajectory>::value;
            static_assert(dimension == 3,
                          "quaternion rotation is only supported for 3D trajectories");
            Trajectory rotated;
            bg::for_each_point(
                    trajectory,
                    [quaternion, &rotated](const Point &point) {
                        static_assert(boost::qvm::is_quat<Quaternion>::value, "Quaternion should be is_quat");
                        static_assert(boost::qvm::is_vec<Point>::value, "Point should be is_vec");
                        static_assert(boost::qvm::vec_traits<Point>::dim==3, "Point should have dim 3");
                        using boost::qvm::operator*;
                        Point rotated_point = quaternion * point;
                        bg::append(rotated, rotated_point);
                    }
            );
            return rotated;
        };
    }

}}

#endif //TRAJECMP_TRANSFORM_SCALE_HPP

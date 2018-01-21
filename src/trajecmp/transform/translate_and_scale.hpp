#ifndef TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP
#define TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP

#include <boost/geometry/algorithms/assign.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>

#include <trajecmp/geometry/point/point.hpp>
#include <trajecmp/geometry/hyper_sphere.hpp>
#include <trajecmp/geometry/min_bounding_sphere.hpp>
#include "map_coordinates.hpp"

namespace trajecmp { namespace transform {

    template<
            typename Trajectory,
            typename Vector = typename boost::geometry::point_type<Trajectory>::type,
            typename Scale = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    void translate_and_scale(const Vector &vector,
                             Scale scale,
                             Trajectory &trajectory) {
        using Point = Vector;
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        for (Point &point : trajectory) {
            map_coordinates_with_index(point, [&](std::size_t i, Coordinate c) {
                using trajecmp::geometry::point::get;
                return (c + get(i, vector)) * scale;
            });
        }
    };

    /**
     * Moves the trajectory so that the center of the MBS (minimum bounding
     * sphere) lies in the origin and scales the trajectory so that the MBS of
     * the transformed trajectory corresponds to the diameter of the MBS.
     * Since the MBS is not passed, it is calculated internally.
     *
     * @tparam Size
     * @tparam Trajectory
     * @param size The diameter of the MBS of the transformed trajectory.
     * @param trajectory The trajectory that should be transformed.
     */
    template<
            typename Size,
            typename Trajectory
    >
    void translate_and_scale_using_mbs(Size size, Trajectory &trajectory) {
        const auto mbs = trajecmp::geometry::min_bounding_sphere(trajectory);
        trajecmp::transform::translate_and_scale(
                trajecmp::geometry::negative_vector_of(mbs.center),
                size / (mbs.radius * 2),
                trajectory
        );
    }

    /**
     * Moves the trajectory so that the center of the MBS (minimum bounding
     * sphere) lies in the origin and scales the trajectory so that the MBS of
     * the transformed trajectory corresponds to the diameter of the MBS.
     * <code>mbs</code> is transformed so that it is the MBS of the transformed
     * trajectory.
     *
     * @tparam Size
     * @tparam Trajectory
     * @param size The diameter of the MBS of the transformed trajectory.
     * @param mbs The minimum bounding sphere of the trajectory before this
     * transformation. It is also transformed so that the center lies in the
     * origin and the radius corresponds to <code>size</code>.
     * @param trajectory The trajectory that should be transformed.
     */
    template<
            typename Size,
            typename Trajectory
    >
    void translate_and_scale_using_mbs(
            Size size,
            trajecmp::geometry::hyper_sphere_of<Trajectory> &mbs,
            Trajectory &trajectory) {
        trajecmp::transform::translate_and_scale(
                trajecmp::geometry::negative_vector_of(mbs.center),
                size / (mbs.radius * 2),
                trajectory
        );
        boost::geometry::assign_zero(mbs.center);
        mbs.radius = size;
    }

    template<
            typename Trajectory,
            typename Scale = typename boost::geometry::coordinate_type<Trajectory>::type,
            typename Vector = typename boost::geometry::point_type<Trajectory>::type
    >
    void scale_and_translate(Scale scale,
                             const Vector &vector,
                             Trajectory &trajectory) {
        using Point = Vector;
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        for (Point &point : trajectory) {
            map_coordinates_with_index(point, [&](std::size_t i, Coordinate c) {
                using trajecmp::geometry::point::get;
                return c * scale + get(i, vector);
            });
        }
    };

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP

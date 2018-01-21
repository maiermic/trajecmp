#ifndef TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP
#define TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP

#include <boost/geometry/algorithms/assign.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>

#include <trajecmp/geometry/point/point.hpp>
#include <trajecmp/geometry/hyper_sphere.hpp>
#include <trajecmp/geometry/min_bounding_sphere.hpp>
#include <trajecmp/trait/concept.hpp>
#include <trajecmp/geometry/min_bounding_box.hpp>
#include <trajecmp/geometry/point/arithmetic.hpp>
#include "map_coordinates.hpp"

namespace trajecmp { namespace transform {

    /**
     * Translate and then scale trajectory using vectors.
     * Note that non-uniform scaling (different scalars for the dimensions)
     * invalidates some previous calculated features like the minimum bounding
     * sphere.
     * They must be recalculated for the transformed trajectory.
     * For uniform scaling use the overload of this function that takes a scalar
     * value instead of a scalar vector.
     *
     * @tparam Trajectory
     * @tparam Vector Has to satisfy the Boost Geometry point concept.
     * @tparam Scale Has to satisfy the Boost Geometry point concept.
     * @param vector Trajectory points are translated by this vector.
     * @param scale Scale for each dimension the trajectory points are scaled
     * by.
     * @param trajectory
     * @return
     */
    template<
            typename Trajectory,
            typename Vector = typename boost::geometry::point_type<Trajectory>::type,
            typename Scale = Vector
    >
    std::enable_if_t<trajecmp::trait::is_point<Scale>>
    translate_and_scale(const Vector &vector,
                        Scale scale,
                        Trajectory &trajectory) {
        using Point = Vector;
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        for (Point &point : trajectory) {
            map_coordinates_with_index(point, [&](std::size_t i, Coordinate c) {
                using trajecmp::geometry::point::get;
                return (c + get(i, vector)) * get(i, scale);
            });
        }
    };

    /**
     * Translate using a translation vector and then scale using a scalar value.
     * For non-uniform scaling use the overload of this function that takes a
     * scalar vector instead of a scalar value.
     *
     * @tparam Trajectory
     * @tparam Vector
     * @tparam Scale
     * @param vector Trajectory points are translated by this vector.
     * @param scale Scalar value that is used to scale each point of the
     * trajectory.
     * @param trajectory
     * @return
     */
    template<
            typename Trajectory,
            typename Vector = typename boost::geometry::point_type<Trajectory>::type,
            typename Scale = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    std::enable_if_t<!trajecmp::trait::is_point<Scale>>
    translate_and_scale(const Vector &vector,
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
     * the transformed trajectory corresponds to the diameter of the MBS and the
     * MBB (minimum bounding box) of the transformed trajectory corresponds to
     * <code>result_mbb</code>.
     * Since the MBS is not passed, it is calculated internally.
     *
     * @tparam Box
     * @tparam Trajectory
     * @param result_mbb The minimum bounding box that the transformed trajectory
     * @param trajectory
     */
    template<
            typename Box,
            typename Trajectory
    >
    void
    translate_using_mbs_and_scale_using_mbb(const Box result_mbb,
                                            Trajectory &trajectory) {
        static_assert(trajecmp::trait::is_box<Box>);
        using Vector = typename boost::geometry::point_type<Trajectory>::type;
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        using trajecmp::geometry::point::operator-;
        using trajecmp::geometry::point::operator/;
        auto mbs = trajecmp::geometry::min_bounding_sphere(trajectory);
        const auto mbb = trajecmp::geometry::min_bounding_box(trajectory);
        translate_and_scale(
                trajecmp::geometry::negative_vector_of(mbs.center),
                (result_mbb.max_corner() - result_mbb.min_corner()) /
                (mbb.max_corner() - mbb.min_corner()),
                trajectory);
        trajectory = translate_by(trajecmp::geometry::negative_vector_of(
                boost::geometry::return_centroid<Vector>(result_mbb)))(trajectory);
    }

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

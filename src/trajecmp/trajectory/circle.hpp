#ifndef TRAJECMP_TRAJECTORY_CIRCLE_HPP
#define TRAJECMP_TRAJECTORY_CIRCLE_HPP

#include <range/v3/view/transform.hpp>
#include <range/v3/to_container.hpp>

#include <boost/concept/assert.hpp>
#include <boost/geometry/geometries/concepts/linestring_concept.hpp>
#include <boost/geometry/arithmetic/arithmetic.hpp>

#include <trajecmp/util/angle.hpp>
#include <trajecmp/range/range.hpp>
#include <trajecmp/range/to_trajectory.hpp>

namespace trajecmp { namespace trajectory {

    template<class Trajectory>
    class circle {
        BOOST_CONCEPT_ASSERT( (boost::geometry::concept::Linestring<Trajectory>) );

        using Point = typename boost::geometry::point_type<Trajectory>::type;
        using Coordinate = typename boost::geometry::coordinate_type<Point>::type;
        using Radius = Coordinate;
        using Angle = Coordinate;

        const Radius _radius;
    public:
        explicit circle(Radius radius) : _radius(radius) {}

        inline Coordinate x(Angle angleDegree) const {
            using trajecmp::util::d2r;
            return std::cos(d2r(angleDegree)) * _radius;
        }

        inline Coordinate y(Angle angleDegree) const {
            using trajecmp::util::d2r;
            return -std::sin(d2r(angleDegree)) * _radius;
        }

        /**
         * Sample trajectory of a circle.
         * The angle starts at the right side of the center of the circle at
         * point (radius, 0).
         * The sampling direction is counterclockwise.
         * Use a negative endAngle to change the sampling direction to
         * clockwise.
         *
         * @param startAngle
         * @param endAngle
         * @param angleStep
         * @return
         */
        inline Trajectory
        sample(Angle startAngle, Angle endAngle, Angle angleStep) const {
            using trajecmp::range::range_bound_always_included;
            using ranges::view::transform;
            return range_bound_always_included(startAngle, endAngle, angleStep) |
                   transform([&](Angle angle) {
                       return Point(x(angle), y(angle));
                   }) |
                   ranges::to_<Trajectory>();
        }

    };

}}

#endif //TRAJECMP_TRAJECTORY_CIRCLE_HPP

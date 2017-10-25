#ifndef TRAJECMP_TRAJECTORY_BEZIERCURVE_HPP
#define TRAJECMP_TRAJECTORY_BEZIERCURVE_HPP

#include <boost/range.hpp>
#include <boost/concept/assert.hpp>
#include <boost/geometry/geometries/concepts/linestring_concept.hpp>
#include <boost/geometry/arithmetic/arithmetic.hpp>

namespace trajecmp { namespace trajectory {

    template<class Trajectory>
    class bezier_curve {
        BOOST_CONCEPT_ASSERT( (boost::geometry::concept::Linestring<Trajectory>) );

        using Point = typename boost::geometry::point_type<Trajectory>::type;
        using Percentage = typename boost::geometry::coordinate_type<Point>::type;

        const Trajectory _points;

        static Point getPt(Point n1, Point n2, Percentage perc) {
            namespace bg = boost::geometry;
            Point result = n2;
            bg::subtract_point(result, n1);
            bg::multiply_value(result, perc);
            bg::add_point(result, n1);
            return result; // n1 + ((n2 - n1) * perc)
        }

    public:
        bezier_curve(std::initializer_list <Point> points)
                : _points(points) {}

        Point atPercentage(Percentage percentage) const {
            Trajectory ps = _points;
            while (ps.size() > 1) {
                Point previous = ps.front();
                for (std::size_t i = 1; i < ps.size(); ++i) {
                    const Point current = ps.at(i);
                    ps.at(i - 1) = getPt(previous, current, percentage);
                    previous = current;
                }
                ps.pop_back();
            }
            return ps.front();
        }

        Trajectory sample(std::size_t numberOfPoints) const {
            Trajectory sampled;
            for (std::size_t i = 0; i <= numberOfPoints; ++i) {
                sampled.push_back(atPercentage(i / Percentage(numberOfPoints)));
            }
            return sampled;
        }

    };

}}

#endif //TRAJECMP_TRAJECTORY_BEZIERCURVE_HPP

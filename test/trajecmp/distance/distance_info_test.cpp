#include <catch.hpp>
#include "../../../example/logging.hpp"

#include <boost/geometry.hpp>

#include "trajecmp/distance/distance_info.hpp"

#include "trajecmp/util/boost_geometry_to_string.hpp"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;
using multi_linestring = bg::model::multi_linestring<linestring>;
using Trajectory = linestring;

TEST_CASE("distance_info", "[BoostGeometry]") {
    using trajecmp::distance::distance_info;
    SECTION("distance of point on trajectory is zero") {
        const Trajectory t {{1.0, 3.0}, {2.0, 3.0}, {2.0, 5.0}};
        boost::geometry::distance_info_result<point> d;

        distance_info(t.at(0), t, d);
        CHECK(d.real_distance == 0.0);
        CHECK(d.fraction1 == 0.0);
        CHECK(d.fraction2 == 0.0);
        CHECK(d.projected_point1 == t.at(0));
        CHECK(d.projected_point2 == t.at(0));

        distance_info(t.at(1), t, d);
        CHECK(d.real_distance == 0.0);
        CHECK(d.fraction1 == 0.0);
        CHECK(d.fraction2 == 1.0);
        CHECK(d.projected_point1 == t.at(1));
        CHECK(d.projected_point2 == t.at(1));

        distance_info(t.at(2), t, d);
        CHECK(d.real_distance == 0.0);
        CHECK(d.fraction1 == 0.0);
        CHECK(d.fraction2 == 1.0);
        CHECK(d.projected_point1 == t.at(2));
        CHECK(d.projected_point2 == t.at(2));
    }
    SECTION("distance to projected point between segment points") {
        const Trajectory t {{1.0, 3.0}, {2.0, 3.0}, {2.0, 5.0}};
        boost::geometry::distance_info_result<point> d;

        point p(1.5, -2.3);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(5.3));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(0.5));
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == point(1.5, 3.0));

        p = point(9.1, 3.6);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(7.1));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(0.3));
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == point(2.0, 3.6));
    }
    SECTION("distance to projected point equals segment point") {
        const Trajectory t {{1.0, 3.0}, {2.0, 3.0}, {2.0, 5.0}};
        boost::geometry::distance_info_result<point> d;

        point p(1.0, -2.3);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(5.3));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(0.0)); // first segment
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == t.at(0));

        p = point(2.0, -2.3);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(5.3));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(1.0)); // first segment
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == t.at(1));

        p = point(1.0, 5.0);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(1.0));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(1.0)); // second segment
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == t.at(2));
    }
    SECTION("distance to projected point outside of segment") {
        const Trajectory t {{1.0, 3.0}, {2.0, 3.0}, {2.0, 5.0}};
        boost::geometry::distance_info_result<point> d;

        point p(-2.0, -1.0);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(5.0));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(0.0)); // first segment
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == t.at(0));

        p = point(5.0, -1.0);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(5.0));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(1.0)); // first segment
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == t.at(1));

        p = point(-1.0, 9.0);
        distance_info(p, t, d);
        CHECK(d.real_distance == Approx(5.0));
        CHECK(d.fraction1 == Approx(0.0));
        CHECK(d.fraction2 == Approx(1.0)); // second segment
        CHECK(d.projected_point1 == p);
        CHECK(d.projected_point2 == t.at(2));
    }
}

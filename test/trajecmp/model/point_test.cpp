#include <trajecmp/model/point.hpp>

#include <boost/concept/assert.hpp>
#include <boost/geometry/geometries/concepts/point_concept.hpp>
#include <boost/qvm/all.hpp>
#include <catch.hpp>


TEST_CASE("point model", "[rotate]") {
    using namespace trajecmp::model;
    SECTION("fulfills Boost Geometry point concept") {
        BOOST_CONCEPT_ASSERT((boost::geometry::concept::Point<point2d>));
        BOOST_CONCEPT_ASSERT((boost::geometry::concept::Point<point3d>));
        BOOST_CONCEPT_ASSERT((boost::geometry::concept::Point<point2f>));
        BOOST_CONCEPT_ASSERT((boost::geometry::concept::Point<point3f>));
    }
    SECTION("fulfills Boost qvm point concept") {
        CHECK(boost::qvm::mag(point2d {2.0, 0.0}) == Approx(2.0));
        CHECK(boost::qvm::mag(point2f {2.0f, 0.0f}) == Approx(2.0f));
    }
    SECTION("default initialization sets all coordinates to zero") {
        CHECK(point2d() == (point2d {0.0, 0.0}));
        CHECK(point3d() == (point3d {0.0, 0.0, 0.0}));
        CHECK(point2f() == (point2f {0.0f, 0.0f}));
        CHECK(point3f() == (point3f {0.0f, 0.0f, 0.0f}));
    }
}

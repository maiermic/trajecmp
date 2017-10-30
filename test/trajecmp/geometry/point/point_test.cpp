#include <vector>

#include <catch.hpp>

#include "trajecmp/model/point.hpp"
#include "trajecmp/geometry/point/point.hpp"
#include "trajecmp/trait/number_type_trait.hpp"


TEST_CASE("trajecmp::geometry::point::get", "[]") {
    using namespace trajecmp::geometry::point;
    using trajecmp::model::point2d;
    using trajecmp::model::point3d;
    SECTION("returns coordinate of valid index") {
        point2d p2(0.0, 1.0);
        CHECK(get(0, p2) == 0.0);
        CHECK(get(1, p2) == 1.0);

        point3d p3(2.0, 3.1, 4.2);
        CHECK(get(0, p3) == 2.0);
        CHECK(get(1, p3) == 3.1);
        CHECK(get(2, p3) == 4.2);
    }
    SECTION("returns first coordinate if index is invalid") {
        point2d p2(0.0, 1.0);
        CHECK(get(2, p2) == 0.0);

        point3d p3(2.0, 3.1, 4.2);
        CHECK(get(3, p3) == 2.0);
        CHECK(get(4, p3) == 2.0);
    }
}

TEST_CASE("trajecmp::geometry::point::set", "[]") {
    using namespace trajecmp::geometry::point;
    using trajecmp::model::point2d;
    using trajecmp::model::point3d;
    SECTION("sets coordinate if index is valid") {
        point2d p2(0.0, 1.0);
        set(0, p2, 0.1);
        CHECK(p2 == point2d(0.1, 1.0));
        set(1, p2, 1.1);
        CHECK(p2 == point2d(0.1, 1.1));

        point3d p3(2.0, 3.1, 4.2);
        set(0, p3, 7.0);
        CHECK(p3 == point3d(7.0, 3.1, 4.2));
        set(1, p3, 8.0);
        CHECK(p3 == point3d(7.0, 8.0, 4.2));
        set(2, p3, 9.0);
        CHECK(p3 == point3d(7.0, 8.0, 9.0));
    }
    SECTION("sets first coordinate if index is invalid") {
        point2d p2(0.0, 1.0);
        set(3, p2, 0.1);
        CHECK(p2 == point2d(0.1, 1.0));

        point3d p3(2.0, 3.1, 4.2);
        set(3, p3, 7.0);
        CHECK(p3 == point3d(7.0, 3.1, 4.2));
    }
}

TEST_CASE("trajecmp::geometry::point::to_point", "[]") {
    using namespace trajecmp::geometry::point;
    using trajecmp::model::point2d;
    using trajecmp::model::point3d;
    SECTION("converts 3D array to point3d") {
        double actual[3] = {2.0, 3.1, 4.2};
        CHECK(to_point<point3d>(actual) == point3d(2.0, 3.1, 4.2));
    }
}

TEST_CASE("trajecmp::geometry::point::equals_approx", "[]") {
    using namespace trajecmp::geometry::point;
    using trajecmp::model::point2d;
    using trajecmp::model::point3d;
    const auto eps =
            trajecmp::trait::number_type_trait<double>::get_default_eps();
    SECTION("equals same points") {
        CHECK(equals_approx(point2d(0.0, 0.0),
                            point2d(0.0, 0.0)));
        CHECK(equals_approx(point2d(-1.0, 3.0),
                            point2d(-1.0, 3.0)));
        CHECK(equals_approx(point2d(10.0, -5.0),
                            point2d(10.0, -5.0)));
        CHECK(equals_approx(point3d(3.0, 10.0, -5.0),
                            point3d(3.0, 10.0, -5.0)));
    }
    SECTION("equals similar points") {
        CHECK(equals_approx(point2d(eps, 0.0),
                            point2d(0.0, -eps)));
        CHECK(equals_approx(point3d(3.0, 10.0, -5.0000000000009099889005823410),
                            point3d(3.0, 10.0, -5.0)));
    }
    SECTION("not equals points with deviation higher than epsilon") {
        CHECK(!equals_approx(point2d(0.1, 0.0),
                            point2d(0.0, 0.001)));
        CHECK(!equals_approx(point2d(eps * 2.0, 0.0),
                            point2d(0.0, -eps * 3.0)));
        CHECK(!equals_approx(point3d(3.0 + eps * 2.0, 10.0, -5.0 - eps),
                            point3d(3.0, 10.0 + eps * 1.1, -5.0)));
    }
}

TEST_CASE("trajecmp::geometry::point::zero", "[]") {
    using namespace trajecmp::geometry::point;
    using trajecmp::model::point2d;
    using trajecmp::model::point3d;
    SECTION("returns zero point") {
        CHECK(zero<point2d>() == point2d(0.0, 0.0));
        CHECK(zero<point3d>() == point3d(0.0, 0.0, 0.0));
    }
}

TEST_CASE("trajecmp::geometry::point::equals_approx_zero", "[]") {
    using namespace trajecmp::geometry::point;
    using trajecmp::model::point2d;
    SECTION("returns zero point") {
        CHECK(equals_approx_zero(point2d(0.0, 0.0000000000009099889005823410)));
    }
}
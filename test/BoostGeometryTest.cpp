#include "BoostGeometryTest.h"
#include <boost/geometry.hpp>

namespace bg = boost::geometry;

TEST_F(BoostGeometryTest, BoostGeometryExample) {
    LOG_MESSAGE("BoostGeometryTest.BoostGeometryExample");

    bg::model::point<double, 2, bg::cs::cartesian> point1;
    bg::model::point<double, 3, bg::cs::cartesian> point2(1.0, 2.0, 3.0);

    bg::set<0>(point1, 1.0);
    point1.set<1>(2.0);

    double x = bg::get<0>(point1);
    double y = point1.get<1>();

    EXPECT_EQ(1.0, x);
    EXPECT_EQ(2.3, y);
}

TEST_F(BoostGeometryTest, linestringExample) {
    LOG_MESSAGE("BoostGeometryTest.linestringExample");

    typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
    typedef bg::model::linestring<point_t> linestring_t;

    linestring_t ls1;
    bg::append(ls1, point_t(0.0, 0.0));
    bg::append(ls1, point_t(1.0, 0.0));
    bg::append(ls1, point_t(1.0, 2.0));

    linestring_t ls2{{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};
//    linestring_t ls2;

    EXPECT_TRUE(matchGeoEq(ls1, ls2));
}

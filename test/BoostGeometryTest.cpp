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
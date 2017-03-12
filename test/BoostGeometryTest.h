#ifndef TRAJECTORY_DETECTION_BOOSTGEOMETRYTEST_H
#define TRAJECTORY_DETECTION_BOOSTGEOMETRYTEST_H

#include <gtest/gtest.h>
#include "TestBase.h"

class BoostGeometryTest : public ::testing::Test {

public:

    BoostGeometryTest() {
        LOG_MESSAGE("Opening test suite: BoostGeometryTest");
    }

    virtual ~BoostGeometryTest() {
        LOG_MESSAGE("Closing test suite: BoostGeometryTest");
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

};


#endif //TRAJECTORY_DETECTION_BOOSTGEOMETRYTEST_H

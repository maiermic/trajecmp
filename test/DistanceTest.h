#ifndef TRAJECTORY_DETECTION_DISTANCE_TEST_H
#define TRAJECTORY_DETECTION_DISTANCE_TEST_H

#include <gtest/gtest.h>
#include "TestBase.h"

class DistanceTest  : public ::testing::Test {

public:

    DistanceTest() {
        LOG_MESSAGE("Opening test suite: DistanceTest");
    }

    virtual ~DistanceTest() {
        LOG_MESSAGE("Closing test suite: DistanceTest");
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

};


#endif //TRAJECTORY_DETECTION_DISTANCE_TEST_H

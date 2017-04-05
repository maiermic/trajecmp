#ifndef TRAJECTORY_DETECTION_INPUT_H
#define TRAJECTORY_DETECTION_INPUT_H

#include <emscripten/bind.h>
#include <sstream>
#include <boost/geometry.hpp>

namespace input {
    using namespace emscripten;
    namespace bg = boost::geometry;

    using point = bg::model::point<double, 2, bg::cs::cartesian>;
    using linestring = bg::model::linestring<point>;
    using Trajectory = linestring;

    Trajectory pattern;

    bool setPattern(const std::string trajectoryString) {
        try {
            const std::string trajectoryWkt("LINESTRING (" + trajectoryString + ")");
            bg::read_wkt(trajectoryWkt, pattern);
            return true;
        } catch (bg::read_wkt_exception e) {
            return false;
        }
    }

    EMSCRIPTEN_BINDINGS(my_module) {
        function("setPattern", &setPattern);
    }
} // namespace input

#endif //TRAJECTORY_DETECTION_INPUT_H

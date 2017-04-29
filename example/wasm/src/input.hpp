#ifndef INPUT_HPP
#define INPUT_HPP

#include <emscripten/bind.h>
#include <sstream>
#include <boost/geometry.hpp>

#include "../../../src/trajecmp/geometry/coordinate.hpp"

namespace input {
    using namespace emscripten;
    namespace bg = boost::geometry;

    using point = bg::model::point<double, 2, bg::cs::cartesian>;
    using linestring = bg::model::linestring<point>;
    using Trajectory = linestring;

    namespace details {
        bool parse_trajectory(const std::string trajectory_string, Trajectory &output) {
            std::istringstream trajectory_stream(trajectory_string);
            std::string coordinates_string;
            while (std::getline(trajectory_stream, coordinates_string, ',')) {
                const char *p = coordinates_string.c_str();
                char *end;
                point current_point;
                const auto dimension = bg::dimension<point>::value;
                int coordinate_index = 0;
                for (double coordinate = std::strtod(p, &end);
                     p != end;
                     coordinate = std::strtod(p, &end),
                     ++coordinate_index
                        ) {
                    if (errno == ERANGE || coordinate_index >= dimension) {
                        return false;
                    }
                    p = end;
                    trajecmp::geometry::set(coordinate_index, current_point, coordinate);
                }
                if (coordinate_index < dimension) {
                    return false;
                }
                bg::append(output, current_point);
            }
            return true;
        }
    }

    Trajectory pattern;

    bool setPattern(const std::string trajectoryString) {
        Trajectory new_pattern;
        if (!details::parse_trajectory(trajectoryString, new_pattern)) {
            return false;
        }
        pattern = new_pattern;
        return true;
    }

    EMSCRIPTEN_BINDINGS(my_module) {
        function("setPattern", &setPattern);
    }
} // namespace input

#endif //INPUT_HPP

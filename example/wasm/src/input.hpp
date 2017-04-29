#ifndef INPUT_HPP
#define INPUT_HPP

#include <emscripten/bind.h>
#include <sstream>
#include <boost/geometry.hpp>

#include "model.hpp"

#include "../../../src/trajecmp/geometry/coordinate.hpp"

namespace input {
    using namespace emscripten;
    namespace bg = boost::geometry;

    namespace details {
        bool parse_trajectory(const std::string trajectory_string, model::Trajectory &output) {
            std::istringstream trajectory_stream(trajectory_string);
            std::string coordinates_string;
            while (std::getline(trajectory_stream, coordinates_string, ',')) {
                const char *p = coordinates_string.c_str();
                char *end;
                model::point current_point;
                const auto dimension = bg::dimension<model::point>::value;
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

    model::Trajectory pattern;

    bool setPattern(const std::string trajectoryString) {
        model::Trajectory new_pattern;
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
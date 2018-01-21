#ifndef PATTERN_HPP
#define PATTERN_HPP

#include "model.hpp"

namespace pattern {

    const model::trajectory letter_L{
            {0, 0},
            {0, 2},
            {1, 2}
    };

    const model::trajectory square{
            {0, 0},
            {0, 1},
            {1, 1},
            {1, 0},
            {0, 0}
    };

    /**
     * Triangle with height and width of 2 and two corners on bottom and
     * one on top.
     */
    const model::trajectory triangle_up{
            {-1.0f, 0.0f},
            {1.0f,  0.0f},
            {0.0f,  2.0f},
            {-1.0f, 0.0f},
    };

} // namespace pattern

#endif //PATTERN_HPP

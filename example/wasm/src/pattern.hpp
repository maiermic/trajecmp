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

} // namespace pattern

#endif //PATTERN_HPP

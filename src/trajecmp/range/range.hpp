#ifndef TRAJECMP_RANGE_RANGE_HPP
#define TRAJECMP_RANGE_RANGE_HPP

#include <cmath>

#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

namespace trajecmp { namespace range {

    /**
     * Range that iterates from one value to another using a specific step size.
     *
     * @tparam Value The type of the values to iterate.
     * @param from Start value (inclusive).
     * @param to End value (inclusive).
     * @param step_size The increment value.
     * @return Range of values.
     */
    template <class Value>
    auto range(Value from, Value to, Value step_size) {
        using std::floor;
        using std::abs;
        using std::invalid_argument;
        if (step_size == Value(0)) {
            throw invalid_argument("step for range must be non-zero");
        } else if (to < from) {
            step_size = -abs(step_size);
        }
        const auto size = (int) floor(abs(to - from) / abs(step_size));
        if (size < 0) {
            throw invalid_argument("size is negative");
        }
        return ranges::view::iota(0, size + 1) |
               ranges::view::transform([=](int i) {
                   return Value(from + i * step_size);
               });
    }

    /**
     * Range that iterates from one value to another using step size one.
     *
     * @tparam Value The type of the values to iterate.
     * @param from Start value (inclusive).
     * @param to End value (inclusive).
     * @return Range of values.
     */
    template <class Value>
    auto range(Value from, Value to) {
        return range(from, to, Value(1));
    }

}}

#endif //TRAJECMP_RANGE_RANGE_HPP

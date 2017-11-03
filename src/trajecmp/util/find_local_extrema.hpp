#ifndef TRAJECMP_UTIL_FIND_LOCAL_EXTREMA_HPP
#define TRAJECMP_UTIL_FIND_LOCAL_EXTREMA_HPP

#include <algorithm>
#include <cstddef>
#include <vector>

namespace trajecmp { namespace util {

    /**
     * Indices of minima and maxima in values passed to find_local_extrema.
     */
    struct extrema {
        std::vector<std::size_t> minima;
        std::vector<std::size_t> maxima;
    };

    /**
     * Find local maxima and minima in a list of values.
     *
     * C++ implementation of:
     * "A Linear-Time Algorithm That Locates Local Extrema
     * of a Function of One Variable From Interval Measurement
     * Results" - Karen Villaverde, Vladik Kreinovich
     * (Interval Computations 01/1993; 1993(4))
     *
     * Takes numbers as first parameter and an accuracy > 0 (epsilon)
     * as second parameter. The accuracy has to be chosen depending
     * on the fluctuations in the data: smaller values mean greater
     * reliability in finding extrema but also greater chance of
     * confusing noise with a local minimum or maximum.
     *
     * @tparam Value The number type of the values.
     * @param values The values in which to search the extrema.
     * @param epsilon The accuracy.
     * @return Object with two lists of indices: 'minima' with indices of
     * values that are local minima and 'maxima' with indices of values that
     * are local maxima.
     * @see https://github.com/balint42/diff.js#extrema-function
     */
    template<typename Value>
    extrema find_local_extrema(const std::vector<Value> &values, Value epsilon) {
        extrema extrema;
        auto s = 0;
        auto m = values.front();
        auto M = values.front();
        auto i = 1;
        std::size_t j;
        while (i < values.size()) {
            if (s == 0) {
                if (!(M - epsilon <= values[i] && values[i] <= m + epsilon)) {
                    if (M - epsilon > values[i]) {
                        s = -1;
                    }
                    if (m + epsilon < values[i]) {
                        s = 1;
                    }
                }
                M = std::max(M, values[i]);
                m = std::min(m, values[i]);
            } else {
                if (s == 1) {
                    if (M - epsilon <= values[i]) {
                        M = std::max(M, values[i]);
                    } else {
                        j = i - 1;
                        while (values[j] >= M - epsilon) {
                            j--;
                        }
                        extrema.maxima.push_back((j + i) / 2);
                        s = -1;
                        m = values[i];
                    }
                } else {
                    if (s == -1) {
                        if (m + epsilon >= values[i]) {
                            m = std::min(m, values[i]);
                        } else {
                            j = i - 1;
                            while (values[j] <= m + epsilon) {
                                j--;
                            }
                            extrema.minima.push_back((j + i) / 2);
                            s = 1;
                            M = values[i];
                        }
                    }
                }
            }
            i++;
        }
        return extrema;
    }

}} // namespace trajecmp::util


#endif //TRAJECMP_UTIL_FIND_LOCAL_EXTREMA_HPP

#ifndef TRAJECMP_ALGORITHM_MINMAX_INDEX_HPP
#define TRAJECMP_ALGORITHM_MINMAX_INDEX_HPP

#include <algorithm>
#include <iterator>
#include <utility>

namespace trajecmp { namespace algorithm {

    /**
     * Version of <code>std::minmax_element</code> that returns indices instead
     * of iterators.
     */
    template<typename Values>
    auto minmax_index(const Values &values) {
        const auto extrema = std::minmax_element(std::begin(values), std::end(values));
        if (extrema.first == std::end(values)) {
            return std::make_pair(-1l, -1l);
        }
        const auto min_index = std::distance(std::begin(values), extrema.first);
        const auto max_index = std::distance(std::begin(values), extrema.second);
        return std::make_pair(min_index, max_index);
    }

    /**
     * Version of <code>std::min_element</code> that returns index instead of
     * iterator.
     */
    template<typename Values>
    auto min_index(const Values &values) {
        const auto min = std::min_element(std::begin(values), std::end(values));
        return (min == std::end(values))
               ? -1l
               : std::distance(std::begin(values), min);
    }

    /**
     * Version of <code>std::max_element</code> that returns index instead of
     * iterator.
     */
    template<typename Values>
    auto max_index(const Values &values) {
        const auto max = std::max_element(std::begin(values), std::end(values));
        return (max == std::end(values))
               ? -1l
               : std::distance(std::begin(values), max);
    }

}} // namespace trajecmp::util


#endif //TRAJECMP_ALGORITHM_MINMAX_INDEX_HPP

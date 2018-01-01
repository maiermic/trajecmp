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

}} // namespace trajecmp::util


#endif //TRAJECMP_ALGORITHM_MINMAX_INDEX_HPP

#ifndef TRAJECMP_ALGORITHM_FIND_INDEX_HPP
#define TRAJECMP_ALGORITHM_FIND_INDEX_HPP

#include <iterator>
#include <range/v3/algorithm/find.hpp>

namespace trajecmp { namespace algorithm {

    /**
     * Version of <code>ranges::find</code> that returns index instead of
     * iterator.
     */
    template<class Values, class Value>
    auto find_index(const Values &values, const Value &value) {
        const auto found = ranges::find(values, value);
        return found == std::end(values)
               ? -1
               : std::distance(std::begin(values), found);
    }

}} // namespace trajecmp::util


#endif //TRAJECMP_ALGORITHM_FIND_INDEX_HPP

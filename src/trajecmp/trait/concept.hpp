#ifndef TRAJECMP_TRAIT_CONCEPT_HPP
#define TRAJECMP_TRAIT_CONCEPT_HPP

#include <type_traits>
#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/core/tag.hpp>

namespace trajecmp { namespace trait {

    template<typename Point>
    constexpr bool is_point = std::is_same
            <
                    typename boost::geometry::tag<Point>::type,
                    boost::geometry::point_tag
            >::type::value;

}}

#endif //TRAJECMP_TRAIT_CONCEPT_HPP

#ifndef TRAJECMP_TRAIT_TYPE_TRAITS_HPP
#define TRAJECMP_TRAIT_TYPE_TRAITS_HPP

#include <type_traits>

namespace trajecmp { namespace trait {

    template<typename T>
    struct remove_cv_ref {
        using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    };

}}

#endif //TRAJECMP_TRAIT_TYPE_TRAITS_HPP

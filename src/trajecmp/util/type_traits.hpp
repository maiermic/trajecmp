#ifndef TRAJECMP_UTIL_TYPE_TRAITS_HPP
#define TRAJECMP_UTIL_TYPE_TRAITS_HPP

#include <type_traits>

namespace trajecmp { namespace util {

    template<typename T>
    struct remove_cv_ref {
        using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    };

}}

#endif //TRAJECMP_UTIL_TYPE_TRAITS_HPP

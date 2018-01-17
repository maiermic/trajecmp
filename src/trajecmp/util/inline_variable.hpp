#ifndef TRAJECMP_UTIL_INLINE_VARIABLE_HPP
#define TRAJECMP_UTIL_INLINE_VARIABLE_HPP

namespace trajecmp { inline namespace util {

    template<typename T>
    struct static_const {
        static constexpr T value{};
    };

    template<typename T>
    constexpr T static_const<T>::value;

}}

#define TRAJECMP_INLINE_VARIABLE(type, name)                              \
    inline namespace function_objects                                     \
    {                                                                     \
        inline namespace                                                  \
        {                                                                 \
            constexpr auto &name = ::trajecmp::static_const<type>::value; \
        }                                                                 \
    }


#endif //TRAJECMP_UTIL_INLINE_VARIABLE_HPP

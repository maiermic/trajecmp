#ifndef TRAJECMP_TRAIT_NUMBER_TYPE_TRAIT_HPP
#define TRAJECMP_TRAIT_NUMBER_TYPE_TRAIT_HPP

namespace trajecmp { namespace trait {

    template<typename LookupTypeT, typename Enable = void>
    struct number_type_trait {
    };

    template<>
    struct number_type_trait<double> {
        static inline double get_default_eps() { return 1E-12; }

        static inline double get_one_element() { return 1.0; }

        static inline double get_zero_element() { return 0.0; }
    };

    template<>
    struct number_type_trait<float> {
        static inline float get_default_eps() { return 1E-30f; }

        static inline float get_one_element() { return 1.0f; }

        static inline float get_zero_element() { return 0.0f; }
    };

}}

#endif //TRAJECMP_TRAIT_NUMBER_TYPE_TRAIT_HPP

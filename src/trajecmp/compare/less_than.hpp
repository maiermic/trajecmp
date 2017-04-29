#ifndef TRAJECMP_COMPARE_LESS_THAN_HPP
#define TRAJECMP_COMPARE_LESS_THAN_HPP


namespace trajecmp { namespace compare {

    template<typename T>
    auto less_than(T upper) {
        return [=](T value) {
            return value <= upper;
        };
    }

}} // namespace trajecmp::compare


#endif //TRAJECMP_COMPARE_LESS_THAN_HPP

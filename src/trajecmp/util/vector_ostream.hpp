#ifndef TRAJECMP_UTIL_VECTOR_OSTREAM_HPP
#define TRAJECMP_UTIL_VECTOR_OSTREAM_HPP

#include <ostream>
#include <vector>
#include <iterator>

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    if (v.empty()) {
        out << "[]";
    } else {
        out << '[';
        std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out << "\b\b]";
    }
    return out;
}

#endif //TRAJECMP_UTIL_VECTOR_OSTREAM_HPP

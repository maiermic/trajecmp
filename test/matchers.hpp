#ifndef TRAJECMP_MATCHERS_HPP
#define TRAJECMP_MATCHERS_HPP

#include <algorithm>
#include <vector>
#include <functional>

#include <trajecmp/geometry/point/point.hpp>
#include <boost/qvm/all.hpp>

template<typename T, typename Compare>
struct CompareMatcher
        : Catch::Matchers::Impl::MatcherBase<std::vector<T>, std::vector<T> > {

    CompareMatcher(const std::vector<T> &comparator, const Compare &compare)
            : m_comparator(comparator),
              m_compare(compare) {}

    bool match(const std::vector<T> &v) const CATCH_OVERRIDE {
        if (m_comparator.size() != v.size()) {
            return false;
        }
        for (size_t i = 0; i < v.size(); ++i) {
            if (!m_compare(m_comparator[i], v[i])) {
                return false;
            }
        }
        return true;
    }

    virtual std::string describe() const CATCH_OVERRIDE {
        return "Equals: " + Catch::toString(m_comparator);
    }

    const std::vector<T> &m_comparator;
    Compare const &m_compare;
};

template<typename T, typename C>
CompareMatcher<T, C>
Compare(const std::vector<T> &comparator, const C &compare) {
    return CompareMatcher<T, C>(comparator, compare);
}

inline auto EqualsApprox(const std::vector<double> &comparator) {
    return Compare(comparator, [=](double actual, double expected) {
        return actual == Approx(expected);
    });
}

template<typename T>
std::vector<T> quat_to_vector(const boost::qvm::quat<T> &q) {
    return {
            boost::qvm::S(q),
            boost::qvm::X(q),
            boost::qvm::Y(q),
            boost::qvm::Z(q),
    };
}

template<typename Point>
bool point_equals_approx(const Point &actual, const Point &expected) {
    static const auto dimension = boost::geometry::dimension<Point>::value;
    for (int i = 0; i < dimension; ++i) {
        if (trajecmp::geometry::point::get(i, actual) !=
            Approx(trajecmp::geometry::point::get(i, expected))) {
            return false;
        }
    }
    return true;
}

template<class Trajectory>
auto TrajectoryEqualsApprox(const Trajectory &comparator) {
    using Point = typename boost::geometry::point_type<Trajectory>::type;
    return Compare(comparator, point_equals_approx<Point>);
}

#endif //TRAJECMP_MATCHERS_HPP

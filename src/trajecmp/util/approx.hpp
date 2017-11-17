#ifndef TRAJECMP_APPROX_HPP
#define TRAJECMP_APPROX_HPP

#include <cstdlib>
#include <limits>
#include <string>
#include <type_traits>
#include <ostream>

namespace trajecmp { namespace util {

    // modified version of Catch::Detail::Approx
    class approx {
    public:
        explicit approx(double value)
                : m_epsilon(std::numeric_limits<float>::epsilon() * 100),
                  m_margin(0.0),
                  m_scale(1.0),
                  m_value(value) {}

        approx(approx const &other)
                : m_epsilon(other.m_epsilon),
                  m_margin(other.m_margin),
                  m_scale(other.m_scale),
                  m_value(other.m_value) {}

        static approx custom() {
            return approx(0);
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        approx operator()(T value) {
            approx approx(static_cast<double>(value));
            approx.epsilon(m_epsilon);
            approx.margin(m_margin);
            approx.scale(m_scale);
            return approx;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        explicit approx(T value): approx(static_cast<double>(value)) {}

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator==(const T &lhs, approx const &rhs) {
            // Thanks to Richard Harris for his help refining this formula
            auto lhs_v = double(lhs);
            bool relativeOK = std::fabs(lhs_v - rhs.m_value) <
                              rhs.m_epsilon * (rhs.m_scale +
                                               (std::max)(std::fabs(
                                                       lhs_v), std::fabs(
                                                       rhs.m_value)));
            if (relativeOK) {
                return true;
            }
            return std::fabs(lhs_v - rhs.m_value) < rhs.m_margin;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator==(approx const &lhs, const T &rhs) {
            return operator==(rhs, lhs);
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator!=(T lhs, approx const &rhs) {
            return !operator==(lhs, rhs);
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator!=(approx const &lhs, T rhs) {
            return !operator==(rhs, lhs);
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator<=(T lhs, approx const &rhs) {
            return double(lhs) < rhs.m_value || lhs == rhs;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator<=(approx const &lhs, T rhs) {
            return lhs.m_value < double(rhs) || lhs == rhs;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator>=(T lhs, approx const &rhs) {
            return double(lhs) > rhs.m_value || lhs == rhs;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        friend bool operator>=(approx const &lhs, T rhs) {
            return lhs.m_value > double(rhs) || lhs == rhs;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        approx &epsilon(T newEpsilon) {
            m_epsilon = double(newEpsilon);
            return *this;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        approx &margin(T newMargin) {
            m_margin = double(newMargin);
            return *this;
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<double, T>::value>::type>
        approx &scale(T newScale) {
            m_scale = double(newScale);
            return *this;
        }

        friend std::ostream &
        operator<<(std::ostream &os, const approx &approx) {
            os << "approx( " << approx.m_value << " )";
            return os;
        }

    private:
        double m_epsilon;
        double m_margin;
        double m_scale;
        double m_value;
    };

}}

#endif //TRAJECMP_APPROX_HPP

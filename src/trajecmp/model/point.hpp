#ifndef TRAJECMP_MODEL_POINT_HPP
#define TRAJECMP_MODEL_POINT_HPP

#include <array>
#include <sstream>

#include <boost/qvm/vec_traits.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>
#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/arithmetic/arithmetic.hpp>


namespace trajecmp { namespace model {

    template<typename CoordinateType, std::size_t DimensionCount>
    struct point : public std::array<CoordinateType, DimensionCount> {
        using self = point<CoordinateType, DimensionCount>;
        using base = std::array<CoordinateType, DimensionCount>;

        point() : std::array<CoordinateType, DimensionCount>() {}

        template<
                typename... Args,
                typename = typename std::enable_if<
                        sizeof...(Args) == DimensionCount
                >::type
        >
        explicit
        point(Args... values)
                : std::array<CoordinateType, DimensionCount>{values...} {
        }

        bool operator==(const self &rhs) const {
            return static_cast<const base &>(*this) ==
                   static_cast<const base &>(rhs);
        }

        bool operator!=(const point &rhs) const {
            return !(rhs == *this);
        }

        friend std::ostream &operator<<(std::ostream &os, const point &point) {
            std::ostringstream oss;
            oss << "{ ";
            auto first = point.begin();
            auto last = point.end();
            if (first != last) {
                oss << (*first);
                for (++first; first != last; ++first)
                    oss << ", " << (*first);
            }
            oss << " }";
            os << oss.str();
            return os;
        }
    };

    using point2d = point<double, 2>;
    using point3d = point<double, 3>;
    using point2f = point<float, 2>;
    using point3f = point<float, 3>;

    template <typename Vector>
    Vector operator-(const Vector lhs, const Vector rhs) {
        Vector result(lhs);
        boost::geometry::subtract_point(result, rhs);
        return result;
    }

}} // trajecmp::model

namespace boost { namespace geometry {

    // Adapt the point to the concept
#ifndef DOXYGEN_NO_TRAITS_SPECIALIZATIONS
    namespace traits {

        template<typename CoordinateType, std::size_t DimensionCount>
        struct tag<trajecmp::model::point<CoordinateType, DimensionCount>> {
            using type = point_tag;
        };

        template<typename CoordinateType, std::size_t DimensionCount>
        struct coordinate_type<trajecmp::model::point<CoordinateType, DimensionCount>> {
            using type = CoordinateType;
        };

        template<typename CoordinateType, std::size_t DimensionCount>
        struct coordinate_system<trajecmp::model::point<CoordinateType, DimensionCount>> {
            using type = boost::geometry::cs::cartesian;
        };

        template<typename CoordinateType, std::size_t DimensionCount>
        struct dimension<trajecmp::model::point<CoordinateType, DimensionCount>>
                : boost::mpl::int_<DimensionCount> {
        };

        template<typename CoordinateType, std::size_t DimensionCount, std::size_t Dimension>
        struct access<trajecmp::model::point<CoordinateType, DimensionCount>, Dimension> {
            static inline CoordinateType
            get(const trajecmp::model::point<CoordinateType, DimensionCount> &v) {
                return v[Dimension];
            }

            static inline void
            set(trajecmp::model::point<CoordinateType, DimensionCount> &v,
                const CoordinateType &value) {
                v[Dimension] = value;
            }
        };

    } // namespace traits
#endif // DOXYGEN_NO_TRAITS_SPECIALIZATIONS

}} // boost::geometry

namespace boost { namespace qvm {

    template<typename CoordinateType, std::size_t DimensionCount>
    struct is_vec<trajecmp::model::point<CoordinateType, DimensionCount>> {
        static bool const value = true;
    };

    template<typename CoordinateType, std::size_t DimensionCount>
    struct vec_traits<trajecmp::model::point<CoordinateType, DimensionCount>> {
        static int const dim = DimensionCount;
        using Point = typename trajecmp::model::point<CoordinateType, DimensionCount>;
        using scalar_type = CoordinateType;

        template<int I>
        static inline scalar_type &write_element(Point &p) {
            return p[I];
        }

        template<int I>
        static inline scalar_type
        read_element(Point const &p) {
            return p[I];
        }
    };

}}


#endif //TRAJECMP_MODEL_POINT_HPP

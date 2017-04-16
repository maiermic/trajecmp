#ifndef TRAJECMP_GEOMETRY_COORDINATE_HPP
#define TRAJECMP_GEOMETRY_COORDINATE_HPP

#include <functional>
#include "point.hpp"

namespace trajecmp { namespace geometry {

    template<class Point>
    class coordinate_iterator {
        using self_t = coordinate_iterator<Point>;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename boost::geometry::coordinate_type<Point>::type;
        using difference_type = std::size_t;
        using pointer = value_type *;
        using reference = value_type &;

        coordinate_iterator()
                : coordinate_iterator(Point()) {}

        coordinate_iterator(Point point)
                : coordinate_iterator(point, 0) {}

        coordinate_iterator(Point point, difference_type pos)
                : m_point(point), m_pos(pos) {}

        inline value_type operator*() {
            return get(m_pos, m_point);
        }

        inline const value_type operator*() const {
            return get(m_pos, m_point);
        }

        inline self_t &operator++() {
            ++m_pos;
            return *this;
        }

        inline self_t operator++(int) {
            self_t copy(*this);
            ++m_pos;
            return copy;
        }

    private:
        Point m_point;
        difference_type m_pos;
    };


    template<typename Geometry>
    struct coordinate_accessor {
        using Pit = typename Geometry::const_iterator;
        using Cit = coordinate_iterator<typename boost::geometry::point_type<Geometry>::type>;

        inline Cit operator()(Pit it) const { return Cit(*it); }
    };

}}// namespace trajecmp::geometry

#endif //TRAJECMP_GEOMETRY_COORDINATE_HPP

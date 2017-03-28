#ifndef TRAJECTORY_DETECTION_COORDINATE_ITERATOR_H
#define TRAJECTORY_DETECTION_COORDINATE_ITERATOR_H

#include <boost/geometry.hpp>

namespace bg = boost::geometry;

template<std::size_t>
struct int2type {
};


template<class Point, std::size_t I>
typename bg::coordinate_type<Point>::type
get_imp(std::size_t index, const Point &point, int2type<I>) {
    return (I == index)
           ? bg::get<I>(point)
           : get_imp(index, point, int2type<I - 1>());
}

template<class Point>
typename bg::coordinate_type<Point>::type
get_imp(std::size_t index, const Point &point, int2type<0>) {
    return bg::get<0>(point);
}

template<class Point>
typename bg::coordinate_type<Point>::type
get(std::size_t index, const Point &point) {
    static std::size_t const size = bg::dimension<Point>::value;
    return get_imp(index, point, int2type<size - 1>());
}


template<class Point, std::size_t I>
void set_imp(std::size_t index,
             Point &point,
             typename bg::coordinate_type<Point>::type value,
             int2type<I>) {
    return (I == index)
           ? bg::set<I>(point, value)
           : set_imp(index, point, value, int2type<I - 1>());
}

template<class Point>
void set_imp(std::size_t index,
             Point &point,
             typename bg::coordinate_type<Point>::type value,
             int2type<0>) {
    return bg::set<0>(point, value);
}

template<class Point>
void set(std::size_t index, Point &point, typename bg::coordinate_type<Point>::type value) {
    static std::size_t const size = bg::dimension<Point>::value;
    return set_imp(index, point, value, int2type<size - 1>());
}


template<class Point>
class CoordinateIterator {
    using self_t = CoordinateIterator<Point>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename bg::coordinate_type<Point>::type;
    using difference_type = std::size_t;
    using pointer = value_type *;
    using reference = value_type &;

private:
    Point _point;
    difference_type _pos;

public:
    CoordinateIterator()
            : CoordinateIterator(Point()) {}

    CoordinateIterator(Point point)
            : CoordinateIterator(point, 0) {}

    CoordinateIterator(Point point, difference_type pos)
            : _point(point), _pos(pos) {}

    inline value_type operator*() {
        return get(_pos, _point);
    }

    inline const value_type operator*() const {
        return get(_pos, _point);
    }

    inline self_t &operator++() {
        ++_pos;
        return *this;
    }

    inline self_t operator++(int) {
        self_t copy(*this);
        ++_pos;
        return copy;
    }
};

#endif //TRAJECTORY_DETECTION_COORDINATE_ITERATOR_H

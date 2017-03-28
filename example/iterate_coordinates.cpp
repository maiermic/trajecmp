#include <boost/geometry.hpp>
#include "../src/Miniball.hpp"

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


template<typename Linestring>
struct CoordinateAccessor {
    using Pit = typename Linestring::const_iterator;
    using Cit = CoordinateIterator<typename bg::point_type<Linestring>::type>;

    inline Cit operator()(Pit it) const { return Cit(*it); }
};


int main(int argc, char *argv[]) {
    using point = bg::model::point<double, 2, bg::cs::cartesian>;
    using linestring = bg::model::linestring<point>;
    using coordinate_type = bg::coordinate_type<linestring>::type;
    using PointIterator = CoordinateAccessor<linestring>::Pit;

    const int dimension = bg::dimension<linestring>::value;
    const int numberOfPoints = 1000000;

    // initialize random number generator
    const double seed = (argc != 2) ? 0 : std::atoi(argv[1]);
    std::srand(seed);

    // generate random points and store them in a linestring
    // ----------------------------------------------------------
    linestring line;
    for (int i = 0; i < numberOfPoints; ++i) {
        point p;
        for (int j = 0; j < dimension; ++j) {
            set(j, p, rand());
        }
        bg::append(line, p);
    }

    // create an instance of Miniball
    // ------------------------------
    using MB = Miniball::Miniball<CoordinateAccessor<linestring>>;
    MB mb(dimension, line.begin(), line.end());

    // output results
    // --------------
    // center
    std::cout << "Center:\n  ";
    const coordinate_type *center = mb.center();
    for (int i = 0; i < dimension; ++i, ++center)
        std::cout << *center << " ";
    std::cout << std::endl;

    // squared radius
    std::cout << "Squared radius:\n  ";
    std::cout << mb.squared_radius() << std::endl;

    // number of support points
    std::cout << "Number of support points:\n  ";
    std::cout << mb.nr_support_points() << std::endl;

    // support points on the boundary determine the smallest enclosing ball
    std::cout << "Support point indices (numbers refer to the input order):\n  ";
    MB::SupportPointIterator it = mb.support_points_begin();
    PointIterator first = line.begin();
    for (; it != mb.support_points_end(); ++it) {
        std::cout << std::distance(first, *it) << " "; // 0 = first point
    }
    std::cout << std::endl;

    // relative error: by how much does the ball fail to contain all points?
    //                 tiny positive numbers come from roundoff and are ok
    std::cout << "Relative error:\n  ";
    coordinate_type suboptimality;
    std::cout << mb.relative_error(suboptimality) << std::endl;

    // suboptimality: by how much does the ball fail to be the smallest
    //                enclosing ball of its support points? should be 0
    //                in most cases, but tiny positive numbers are again ok
    std::cout << "Suboptimality:\n  ";
    std::cout << suboptimality << std::endl;

    // validity: the ball is considered valid if the relative error is tiny
    //           (<= 10 times the machine epsilon) and the suboptimality is zero
    std::cout << "Validity:\n  ";
    std::cout << (mb.is_valid() ? "ok" : "possibly invalid") << std::endl;

    // computation time
    std::cout << "Computation time was " << mb.get_time() << " seconds\n";

    return 0;
}
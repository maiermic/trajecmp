#include <cstdlib>
#include <iostream>
#include <vector>
#include <list>
#include <iterator>

#include <boost/geometry.hpp>

#include "../src/coordinate_iterator.h"
#include "../src/Miniball.hpp"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;
using multi_linestring = bg::model::multi_linestring<linestring>;
using Trajectory = linestring;

template <typename Linestring>
struct CoordinateAccessor {
    using Pit = typename Linestring::const_iterator;
    using Cit = CoordinateIterator<typename bg::point_type<Linestring>::type>;
    inline Cit operator() (Pit it) const { return Cit(*it); }
};

int main (int argc, char* argv[])
{
    using coordinate_type = typename bg::coordinate_type<Trajectory>::type;

    const int dimension = bg::dimension<Trajectory>::value;

    Trajectory trajectory {
            {0, 2},
            {0, 0},
            {1, 0}
    };

    // create an instance of Miniball
    // ------------------------------
    using MinBoundingSphere = Miniball::Miniball <CoordinateAccessor<Trajectory>>;
    MinBoundingSphere mb (dimension, trajectory.begin(), trajectory.end());

    // output results
    // --------------
    // center
    std::cout << "Center:\n  ";
    const coordinate_type* center = mb.center();
    for(int i=0; i<dimension; ++i, ++center)
        std::cout << *center << " ";
    std::cout << std::endl;

    // squared radius
    std::cout << "Squared radius:\n  ";
    std::cout << mb.squared_radius() <<  std::endl;

    return 0;
}
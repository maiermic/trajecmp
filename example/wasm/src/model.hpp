#ifndef MODEL_HPP
#define MODEL_HPP

#include <boost/geometry.hpp>

namespace model {
    using point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
    using trajectory = boost::geometry::model::linestring<point>;
    using vector = point;
} // namespace model

#endif //MODEL_HPP

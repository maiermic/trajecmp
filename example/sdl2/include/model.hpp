#ifndef MODEL_HPP
#define MODEL_HPP

#include <boost/geometry.hpp>

namespace model {
    using point = boost::geometry::model::point<float, 2, boost::geometry::cs::cartesian>;
    using trajectory = boost::geometry::model::linestring<point>;
    using vector = point;

    vector operator-(const vector lhs, const vector rhs) {
        vector result(lhs);
        boost::geometry::subtract_point(result, rhs);
        return result;
    }

} // namespace model

#endif //MODEL_HPP

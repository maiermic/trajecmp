#ifndef TRAJECMP_MODEL_TRAJECTORY_HPP
#define TRAJECMP_MODEL_TRAJECTORY_HPP

#include <trajecmp/model/point.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>

namespace trajecmp { namespace model {
    using trajectory2d = std::vector<point2d>;
    using trajectory3d = std::vector<point3d>;
    using trajectory2f = std::vector<point2f>;
    using trajectory3f = std::vector<point3f>;
}}

BOOST_GEOMETRY_REGISTER_LINESTRING(trajecmp::model::trajectory2d);
BOOST_GEOMETRY_REGISTER_LINESTRING(trajecmp::model::trajectory3d);
BOOST_GEOMETRY_REGISTER_LINESTRING(trajecmp::model::trajectory2f);
BOOST_GEOMETRY_REGISTER_LINESTRING(trajecmp::model::trajectory3f);

#endif //TRAJECMP_MODEL_TRAJECTORY_HPP

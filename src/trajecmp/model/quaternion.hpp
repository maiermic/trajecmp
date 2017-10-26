#ifndef TRAJECMP_MODEL_QUATERNION_HPP
#define TRAJECMP_MODEL_QUATERNION_HPP

#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/qvm/quat_operations.hpp>


namespace trajecmp { namespace model {

    template <typename Point>
    using quaternion_type = boost::qvm::qvm_detail::rot_quat_<Point>;

    template<
            typename DegreeOrRadian = boost::geometry::degree,
            typename Point = void,
            typename Angle = typename boost::geometry::coordinate_type<Point>::type
    >
    quaternion_type<Point> quaternion(const Point &axis, Angle angle) {
        return boost::qvm::rot_quat(axis, angle);
    };

}}

#endif //TRAJECMP_MODEL_QUATERNION_HPP

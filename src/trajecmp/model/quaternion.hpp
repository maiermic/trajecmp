#ifndef TRAJECMP_MODEL_QUATERNION_HPP
#define TRAJECMP_MODEL_QUATERNION_HPP

#include <boost/geometry/algorithms/detail/normalize.hpp>
#include <boost/geometry/arithmetic/dot_product.hpp>
#include <boost/geometry/extensions/arithmetic/cross_product.hpp>
#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/util/math.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/quat_operations.hpp>

#include <trajecmp/trait/number_type_trait.hpp>

namespace trajecmp { namespace model {

    template<typename CalculationType>
    using quaternion_type = boost::qvm::quat<CalculationType>;

    /**
     * Create a quaternion that rotates about an axis.
     *
     * @tparam DegreeOrRadian Type of rotation angle specification (degree or
     * radian).
     * @tparam Axis Type of the vector used as rotation axis.
     * Has to fulfill the point concept.
     * @tparam Angle Type of angle has to be compatible with the coordinate type
     * of the axis.
     * @param axis The rotation axis.
     * @param angle The angle of the rotation.
     * @return Quaternion that rotates about an axis.
     */
    template<
            typename DegreeOrRadian = boost::geometry::degree,
            typename Axis = void,
            typename Angle = typename boost::geometry::coordinate_type<Axis>::type
    >
    inline quaternion_type<typename boost::geometry::coordinate_type<Axis>::type>
    quaternion(const Axis &axis, Angle angle) {
        return boost::qvm::rot_quat(axis, angle);
    };

    /**
     * Create quaternion that rotates from one direction vector to another.
     * @tparam Vector The type of the direction vectors.
     * Has to fulfill the point concept.
     * @param rotateFrom The direction vector where the rotation begins.
     * @param rotateTo The direction vector where the rotation ends.
     * @return Quaternion that rotates from one direction vector to another.
     */
    template<typename Vector>
    inline quaternion_type<typename boost::geometry::coordinate_type<Vector>::type>
    quaternion(const Vector &rotateFrom,
               const Vector &rotateTo) {
        namespace bg = boost::geometry;
        using bg::math::sqrt;
        using Value = typename bg::coordinate_type<Vector>::type;
        using ValueT = trajecmp::trait::number_type_trait<Value>;
        const Value pi = bg::math::pi<Value>();
        Vector from = rotateFrom;
        Vector to = rotateTo;

        bg::detail::normalize(from, from);
        bg::detail::normalize(to, to);

        Value cost = bg::dot_product(from, to);

        // check for degeneracies
        if (cost > (ValueT::get_one_element() -
                    ValueT::get_default_eps())) {   // vectors are parallel
            // return identity
            Vector axis;
            // axis should not be (0, 0, 0)
            bg::set<0>(axis, ValueT::get_one_element());
            return quaternion(axis, ValueT::get_zero_element());
        } else if (cost < (-ValueT::get_one_element() +
                           ValueT::get_default_eps())) {
            // vectors are opposite
            // find an axis to rotate around, which should be
            // perpendicular to the original axis
            // Try cross product with (1,0,0) first, if that's one of our
            // original vectors then try  (0,1,0).

            Vector cAxis(1.0, 0.0, 0.0);

            Vector tmp = bg::cross_product(from, cAxis);

            if (boost::qvm::mag(tmp) < 0.00001) {
                cAxis = Vector(0.0, 1.0, 0.0);

                tmp = bg::cross_product(from, cAxis);
            }

            bg::detail::normalize(tmp, tmp);

            return quaternion(tmp, pi);
        }

        Vector axis = bg::cross_product(rotateFrom, rotateTo);
        bg::detail::normalize(axis, axis);

        // use half-angle formulae
        // sin^2 t = ( 1 - cos (2t) ) / 2
        bg::multiply_value(axis, sqrt<Value>(0.5 * (1.0 - cost)));
        // ^ scale the axis by the sine of half the rotation angle to get
        // the normalized quaternion

        return {
                // cos^2 t = ( 1 + cos (2t) ) / 2
                // w part is cosine of half the rotation angle
                sqrt<Value>(0.5 * (1.0 + cost)),

                bg::get<0>(axis),
                -bg::get<1>(axis),
                bg::get<2>(axis),
        };
    }
}}

#endif //TRAJECMP_MODEL_QUATERNION_HPP

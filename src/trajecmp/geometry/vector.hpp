#ifndef TRAJECMP_GEOMETRY_VECTOR_HPP
#define TRAJECMP_GEOMETRY_VECTOR_HPP

#include <functional>

namespace trajecmp { namespace geometry {

    template<class Vector>
    Vector negative_vector_of(const Vector &vector) {
        Vector negative_vector(vector);
        boost::geometry::multiply_value(negative_vector, -1);
        return negative_vector;
    }

}}// namespace trajecmp::geometry

#endif //TRAJECMP_GEOMETRY_VECTOR_HPP

#include <model.hpp>

namespace model {

    vector operator-(const vector lhs, const vector rhs) {
        vector result(lhs);
        boost::geometry::subtract_point(result, rhs);
        return result;
    }

} // namespace model

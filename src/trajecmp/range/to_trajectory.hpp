#ifndef TRAJECMP_RANGE_TO_TRAJECTORY_HPP
#define TRAJECMP_RANGE_TO_TRAJECTORY_HPP

#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

namespace trajecmp { namespace range {

    /**
     * Converter that inserts all values of a range into a trajectory.
     *
     * @tparam Trajectory The type of trajectory that should be created by the
     * converter.
     */
    template<class Trajectory>
    struct to_trajectory {
        template<class Range>
        Trajectory operator()(const Range &values) const {
            Trajectory result;
            boost::range::insert(result, result.begin(), values);
            return result;
        }
    };

    template<typename SinglePassRange, class Trajectory>
    inline Trajectory
    operator|(const SinglePassRange &rng,
              const to_trajectory<Trajectory> f) {
        return f(rng);
    }

}}

#endif //TRAJECMP_RANGE_TO_TRAJECTORY_HPP

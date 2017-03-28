#include <iostream>
#include <fstream>
#include <boost/assign.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <boost/geometry/io/wkt/read.hpp>
#include "../src/TrajectorySvg.h"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;

int main() {
    linestring trajectory_1;
    bg::read_wkt(
            "LINESTRING (63 451, 63 450, 63 449, 63 448, 63 448, 63 448, 63 447, 63 446, 63 445, 63 445, 63 445, 63 444, 63 443, 63 442, 63 442, 63 442, 63 441, 63 440, 63 436, 63 433, 63 428, 63 425, 63 421, 63 420, 63 416, 63 414, 63 411, 63 410, 63 406, 63 406, 63 405, 63 404, 63 402, 62 401, 61 400, 61 398, 60 395, 60 394, 59 393, 59 388, 59 387, 59 383, 59 382, 59 379, 59 378, 59 377, 59 376, 59 375, 59 374, 59 373, 59 373, 59 372, 59 372, 59 372, 59 372, 59 371, 59 368, 59 367, 59 366, 58 365, 58 365, 58 364, 58 363, 58 359, 58 354, 58 350, 58 347, 58 341, 58 337, 58 334, 58 331, 58 328, 58 325, 58 324, 58 322, 58 321, 58 317, 58 316, 58 315, 58 310, 58 309, 58 308, 59 308, 59 307, 59 307, 60 307, 63 306, 64 305, 67 305, 73 301, 73 300, 78 300, 79 299, 80 298, 81 298, 81 298, 82 298, 85 298, 88 298, 92 298, 93 298, 94 298, 94 298, 95 298, 98 298, 101 298, 104 298, 105 297, 106 297, 106 297, 105 297)",
            trajectory_1
    );
    linestring trajectory_2;
    bg::read_wkt(
            "LINESTRING (424 237, 424 236, 424 235, 424 234, 424 229, 424 226, 424 221, 424 216, 424 211, 424 207, 424 203, 424 199, 424 194, 424 189, 424 184, 424 180, 424 174, 424 169, 424 164, 424 159, 424 155, 424 151, 423 146, 423 142, 423 139, 423 137, 422 132, 422 129, 421 126, 421 123, 420 119, 420 116, 420 114, 420 111, 419 110, 419 107, 419 106, 419 105, 419 102, 419 101, 419 99, 419 98, 419 97, 419 96, 419 95, 419 95, 419 94, 419 91, 419 90, 419 89, 419 88, 419 87, 419 87, 419 87, 417 87, 412 87, 408 87, 396 87, 388 88, 380 88, 370 89, 360 89, 352 89, 344 91, 337 92, 332 93, 327 94, 326 95, 325 95, 324 95, 324 96, 324 96, 325 96, 325 96, 326 96, 326 96, 326 96, 327 96, 327 96)",
            trajectory_2
    );
    linestring trajectory_1_simplified;
    bg::simplify(trajectory_1, trajectory_1_simplified, 10.0);

    TrajectorySvg svg("make_envelope.svg", 500, 500);
    svg.add(trajectory_1, "trajectory_1");
    svg.add(trajectory_1_simplified, "trajectory_1_simplified");
    svg.add(trajectory_2, "trajectory_2");
}
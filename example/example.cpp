#include <iostream>

#include <boost/geometry.hpp>

#include "trajecmp/distance/modified_hausdorff.hpp"
#include "trajecmp/geometry/min_bounding_sphere.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"

#include "logging.hpp"
#include "TrajectorySvg.hpp"


int main() {
    namespace bg = boost::geometry;
    namespace trans = boost::geometry::strategy::transform;

    using point = bg::model::point<double, 2, bg::cs::cartesian>;
    using linestring = bg::model::linestring<point>;
    using Trajectory = linestring;

    using trajecmp::geometry::min_bounding_sphere;

    logging::is_logging = true;

//    const Trajectory input_trajectory{{10, 4},
//                                      {10, 0},
//                                      {12.1, 0}};
    Trajectory input_trajectory;
    bg::read_wkt(
//            "LINESTRING (94 467, 94 467, 94 466, 94 466, 94 465, 94 465, 94 465, 94 464, 94 464, 94 463, 95 462, 95 461, 95 461, 95 459, 95 458, 95 457, 95 455, 95 454, 95 452, 95 451, 95 449, 95 447, 95 445, 95 443, 96 441, 96 439, 96 437, 96 434, 97 432, 97 429, 97 427, 97 424, 97 421, 97 418, 97 416, 97 413, 97 411, 97 409, 97 406, 97 405, 97 402, 97 399, 97 397, 96 394, 96 391, 96 389, 95 386, 95 383, 95 381, 95 378, 95 375, 95 372, 95 370, 95 367, 95 363, 95 360, 95 356, 95 353, 95 349, 95 345, 95 341, 95 337, 95 333, 95 329, 95 325, 95 321, 95 316, 95 312, 96 308, 96 303, 96 299, 96 294, 96 289, 96 285, 96 281, 96 276, 97 271, 97 267, 97 262, 97 257, 97 252, 97 247, 97 242, 97 237, 97 233, 98 228, 98 223, 98 219, 98 214, 98 210, 98 205, 98 200, 98 196, 98 191, 98 187, 98 183, 98 178, 98 174, 98 170, 98 166, 99 163, 99 159, 99 156, 99 152, 99 148, 100 144, 100 141, 100 138, 100 134, 100 132, 101 129, 101 127, 101 125, 101 123, 101 121, 101 119, 101 118, 101 116, 101 115, 102 114, 101 113, 101 113, 101 112, 101 112, 101 111, 102 111, 102 111, 102 111, 102 111, 102 111, 102 111, 102 111, 102 111, 101 110, 101 110, 101 109, 101 109, 101 109, 101 108, 101 107, 101 107, 101 107, 100 106, 100 105, 100 104, 100 103, 99 103, 99 102, 99 101, 98 100, 98 99, 98 99, 98 98, 98 97, 98 96, 98 95, 97 95, 97 93, 97 93, 97 92, 97 91, 97 90, 96 89, 96 89, 96 88, 96 86, 96 85, 96 84, 95 83, 95 82, 95 81, 95 81, 95 80, 95 79, 95 76, 94 75, 94 75, 94 74, 94 73, 94 72, 94 72, 94 71, 94 70, 94 69, 94 69, 94 68, 94 68, 94 68, 94 67, 94 67, 94 66, 94 66, 94 65, 93 64, 93 63, 93 62, 93 61, 93 60, 92 58, 92 57, 92 56, 92 56, 92 56, 92 56, 92 55, 92 55, 92 55, 92 54, 93 54, 93 54, 94 54, 95 54, 95 54, 96 54, 97 54, 98 54, 98 54, 100 54, 101 53, 102 52, 103 52, 106 50, 108 49, 109 48, 111 47, 113 47, 116 46, 118 45, 120 43, 123 42, 129 39, 132 37, 136 35, 139 34, 148 31, 152 29, 156 29, 160 27, 164 26, 168 25, 171 24, 181 22, 186 22, 188 22, 190 22, 192 21, 195 21, 196 21, 197 21, 198 21, 198 22, 199 22, 200 23, 201 24, 202 25, 203 26, 207 28, 208 29, 210 30, 214 31, 215 32, 218 32, 223 33, 229 34, 236 33, 240 34, 244 33, 252 33, 257 33, 261 33, 269 33, 285 33, 288 33, 294 32, 296 31, 299 31, 303 30, 306 29, 307 29, 307 29, 308 29, 308 30, 309 30, 310 31, 311 31, 312 31, 312 31, 313 32, 314 31, 314 31, 315 31, 316 32, 316 32, 317 32, 318 32, 319 33, 320 33, 320 33, 321 34, 322 33, 323 34, 324 35, 326 36, 329 37, 332 38, 334 39, 336 39, 337 39, 339 39, 339 39, 340 39, 341 39, 342 40, 342 39, 343 40, 344 40, 345 40, 345 40, 346 41, 347 41, 347 40, 348 40, 349 41, 349 41, 350 41, 352 40, 353 40, 354 39, 356 39, 358 39, 359 40, 360 40, 360 40, 361 40, 362 40, 362 40, 363 40, 363 40, 363 40, 364 40, 364 39, 363 37)",
            "LINESTRING (128 405, 128 405, 128 405, 128 405, 128 405, 128 405, 128 405, 129 404, 129 404, 129 404, 129 404, 129 404, 129 404, 129 403, 129 403, 129 403, 129 402, 129 401, 129 400, 129 399, 129 398, 130 397, 130 395, 130 394, 130 393, 130 391, 130 389, 130 387, 130 385, 130 383, 130 381, 129 379, 129 377, 129 375, 129 372, 128 370, 128 368, 128 366, 128 364, 127 361, 127 360, 127 358, 127 356, 127 355, 127 353, 127 351, 127 349, 127 347, 126 346, 126 344, 126 342, 126 340, 126 338, 126 337, 126 335, 126 333, 125 331, 125 329, 125 327, 125 325, 125 323, 125 321, 125 319, 125 317, 125 315, 125 312, 125 309, 124 307, 124 304, 124 300, 124 297, 124 294, 124 291, 123 287, 123 284, 123 281, 123 278, 123 275, 122 273, 122 270, 122 267, 121 264, 121 262, 120 259, 120 256, 120 254, 119 251, 119 249, 119 247, 119 246, 119 244, 119 242, 118 241, 118 239, 118 238, 118 237, 118 236, 118 235, 117 235, 117 235, 117 235, 117 235, 117 235, 117 235, 117 235, 117 235, 117 235, 118 234, 119 234, 119 233, 120 233, 122 234, 123 234, 125 234, 127 234, 130 234, 132 234, 135 235, 138 235, 140 235, 144 235, 147 236, 151 236, 154 237, 158 237, 161 238, 165 238, 168 238, 171 238, 174 238, 176 238, 178 238, 180 238, 181 238, 182 238, 183 238, 183 238, 184 238, 184 238, 184 238, 185 238, 185 237, 186 237, 186 237, 187 236, 187 236, 188 236, 189 235, 190 234, 191 234, 191 233, 192 232, 193 232, 194 232, 194 231, 195 231, 196 231, 197 231, 197 230, 198 230, 198 230, 198 230, 199 230, 199 230, 199 230, 199 230, 199 230, 199 230, 199 230, 199 230, 199 230, 199 230, 199 230, 198 230, 198 231, 197 232)",
            input_trajectory
    );

    // define pattern trajectory
    const Trajectory pattern_letter_L{{0, 2},
                                      {0, 0},
                                      {1, 0}};

    const Trajectory &pattern_trajectory = pattern_letter_L;

    // --------------
    // preprocessing
    // --------------

    const auto input_mbs = min_bounding_sphere(input_trajectory);
    const auto pattern_mbs = min_bounding_sphere(pattern_trajectory);


    // translate
    point pattern_translation_vector(pattern_mbs.center);
    bg::multiply_value(pattern_translation_vector, -1);
    LOG(pattern_translation_vector);

    Trajectory pattern_translated;
    trans::translate_transformer<double, 2, 2> pattern_translate(bg::get<0>(pattern_translation_vector),
                                                                 bg::get<1>(pattern_translation_vector));
    boost::geometry::transform(pattern_trajectory, pattern_translated, pattern_translate);
    LOG(pattern_translated);


    point input_translation_vector(input_mbs.center);
    bg::multiply_value(input_translation_vector, -1);
    LOG(input_translation_vector);

    Trajectory input_translated;
    trans::translate_transformer<double, 2, 2> input_translate(bg::get<0>(input_translation_vector),
                                                               bg::get<1>(input_translation_vector));
    boost::geometry::transform(input_trajectory, input_translated, input_translate);
    LOG(input_translated);


    // scale
    const double normalized_size = 100;

    Trajectory pattern_scaled;
    trans::scale_transformer<double, 2, 2> pattern_scale(normalized_size / pattern_mbs.radius);
    boost::geometry::transform(pattern_translated, pattern_scaled, pattern_scale);
    LOG(pattern_scaled);

    Trajectory input_scaled;
    trans::scale_transformer<double, 2, 2> input_scale(normalized_size / input_mbs.radius);
    boost::geometry::transform(input_translated, input_scaled, input_scale);
    LOG(input_scaled);


    // rotate
    // no


    const Trajectory &transformed_input = input_scaled;
    const Trajectory &transformed_pattern = pattern_scaled;

    // ---------------------
    // compare trajectories
    // ---------------------

    // configure distance measurement
    trajecmp::distance::neighbours_percentage_range neighbours(0.1);

    // calculate distance
    const auto distance = trajecmp::distance::modified_hausdorff(neighbours)(transformed_input, transformed_pattern);
    LOG(distance);

    const auto max_distance = normalized_size * 0.12;
    LOG(max_distance);

    // decide similarity based on distance
    const auto is_similar = (distance <= max_distance);

    LOG(is_similar);


    // ---------------------
    // visualize
    // ---------------------

    const auto visualization_size = 500;

    Trajectory input_visualization_scaled;
    trans::scale_transformer<double, 2, 2> input_visualization_scale(visualization_size / 2 / normalized_size);
    boost::geometry::transform(transformed_input, input_visualization_scaled, input_visualization_scale);

    Trajectory input_visualization_translated;
    trans::translate_transformer<double, 2, 2> input_visualization_translate(visualization_size / 2,
                                                                             visualization_size / 2);
    boost::geometry::transform(input_visualization_scaled,
                               input_visualization_translated,
                               input_visualization_translate);


    Trajectory pattern_visualization_scaled;
    trans::scale_transformer<double, 2, 2> pattern_visualization_scale(visualization_size / 2 / normalized_size);
    boost::geometry::transform(transformed_pattern,
                               pattern_visualization_scaled,
                               pattern_visualization_scale);

    Trajectory pattern_visualization_translated;
    trans::translate_transformer<double, 2, 2> pattern_visualization_translate(visualization_size / 2,
                                                                               visualization_size / 2);
    boost::geometry::transform(pattern_visualization_scaled,
                               pattern_visualization_translated,
                               pattern_visualization_translate);


    const Trajectory &visualization_normalized_input = input_visualization_translated;
    const Trajectory &visualization_normalized_pattern = pattern_visualization_translated;
    const auto visualization_normalized_input_mbs = min_bounding_sphere(visualization_normalized_input);
    const auto visualization_normalized_pattern_mbs = min_bounding_sphere(visualization_normalized_pattern);
    LOG(visualization_normalized_input_mbs);
    LOG(visualization_normalized_pattern_mbs);

    TrajectorySvg svg("example.svg", visualization_size, visualization_size);
    svg.add(visualization_normalized_input, "visualization_normalized_input");
    svg.add(visualization_normalized_pattern, "visualization_normalized_pattern");
}
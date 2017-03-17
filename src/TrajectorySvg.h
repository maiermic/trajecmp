#ifndef TRAJECTORY_DETECTION_TRAJECTORYSVG_H
#define TRAJECTORY_DETECTION_TRAJECTORYSVG_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/geometry.hpp>
#include "DistinctColorSet.h"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;
using multi_linestring = bg::model::multi_linestring<linestring>;
using Trajectory = linestring;

linestring box(double x, double y, double width, double height) {
    return linestring {
            {x,         y},
            {x + width, y},
            {x + width, y + height},
            {x,         y + height},
            {x,         y}
    };
}

class TrajectorySvg {
    using point_type = typename boost::geometry::point_type<Trajectory>::type;
    using SvgMapper = boost::geometry::svg_mapper<point_type>;

    std::ofstream _file;
    SvgMapper _mapper;
    DistinctColorSet _colorSet;
public:
    TrajectorySvg(std::string const &filename, int width, int height)
            : _file(filename.c_str()), _mapper(_file, width, height) {
        auto viewBox = box(0.0, 0.0, width, height);
        _mapper.add(viewBox);
        _mapper.map(viewBox, "stroke:rgb(100, 100, 100);stroke-width:1;stroke-dasharray:1,3");
    }

    void add(Trajectory const &trajectory) {
        std::ostringstream style;
        style << "stroke-width:2;stroke:" << _colorSet.next();
        _mapper.add(trajectory);
        _mapper.map(trajectory, style.str());
    }
};


#endif //TRAJECTORY_DETECTION_TRAJECTORYSVG_H

#ifndef TRAJECTORY_DETECTION_TRAJECTORYSVG_H
#define TRAJECTORY_DETECTION_TRAJECTORYSVG_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/geometry.hpp>
#include "DistinctColorSet.hpp"

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

    const int _height;
    linestring _symbol_map_line;
    point_type _symbol_map_text_position;
    const bg::strategy::transform::translate_transformer<double, 2, 2> _translate;
    std::ofstream _file;
    SvgMapper _mapper;
    DistinctColorSet _colorSet;

    TrajectorySvg(
        std::string const &filename, int width, int height,
        double symbol_map_padding_left,
        double symbol_map_padding_top,
        double symbol_map_line_length,
        double text_height)
            : _file(filename.c_str())
            , _mapper(_file, width, height)
            , _symbol_map_line {
                    {
                            symbol_map_padding_left,
                            height - symbol_map_padding_top + 4.0
                    },
                    {
                            symbol_map_padding_left + symbol_map_line_length,
                            height - symbol_map_padding_top + 4.0
                    }
            }
            , _symbol_map_text_position(symbol_map_padding_left + symbol_map_line_length + 2.0, height - symbol_map_padding_top)
            , _translate(0.0, -20.0)
            , _height(height) {
        auto viewBox = box(0.0, 0.0, width, height);
        _mapper.add(viewBox);
        _mapper.map(viewBox, "stroke:rgb(100, 100, 100);stroke-width:1;stroke-dasharray:1,3");
    }

    template <class Geoemtry>
    void translate(Geoemtry & geometry) {
        Geoemtry translated;
        bg::transform(geometry, translated, _translate);
        geometry = translated;
    }
public:
    TrajectorySvg(std::string const &filename, int width, int height)
            : TrajectorySvg(filename, width, height, 10.0, 20.0, 5.0, 18.0) {}


    void add(Trajectory const &trajectory, std::string const &description) {
        std::ostringstream style;
        style << "stroke-width:2;stroke:" << _colorSet.next();

        // symbol map
        _mapper.add(_symbol_map_line);
        _mapper.map(_symbol_map_line, style.str());
        translate(_symbol_map_line);
        _mapper.text(_symbol_map_text_position, description, "font-size:18");
        translate(_symbol_map_text_position);

        _mapper.add(trajectory);
        _mapper.map(trajectory, style.str());
    }
};


#endif //TRAJECTORY_DETECTION_TRAJECTORYSVG_H

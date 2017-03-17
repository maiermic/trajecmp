#include <iostream>
#include <fstream>
#include <boost/assign.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

#include <boost/geometry/io/svg/svg_mapper.hpp>

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;

template <typename Geometry1, typename Geometry2>
void create_svg(std::string const& filename, Geometry1 const& first, Geometry2 const& second)
{
    typedef typename boost::geometry::point_type<Geometry1>::type point_type;
    std::ofstream svg(filename.c_str());

    boost::geometry::svg_mapper<point_type> mapper(svg, 400, 400);
    mapper.add(first);
    mapper.add(second);

    const auto first_style = "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2";
    const auto second_style = "opacity:0.8;fill:none;stroke:rgb(255,128,0);stroke-width:4;stroke-dasharray:1,7;stroke-linecap:round";

    mapper.map(first, first_style);
    mapper.map(second, second_style);

    const linestring first_symbol_map_line { {0.0, 1.0}, {0.3, 1.0} };
    mapper.add(first_symbol_map_line);
    mapper.map(first_symbol_map_line, first_style);
    mapper.text(point(0.4, 1.0), "first", "font-size:18");

    const linestring second_symbol_map_line { {0.0, 0.8}, {0.3, 0.8} };
    mapper.add(second_symbol_map_line);
    mapper.map(second_symbol_map_line, second_style);
    mapper.text(point(0.4, 0.8), "second", "font-size:18");
}

int main()
{
    linestring trajectory_1 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};
    linestring trajectory_2 {{0.0, 0.0}, {0.8, 0.1}, {0.9, 1.9}};
    create_svg("make_envelope.svg", trajectory_1, trajectory_2);
}
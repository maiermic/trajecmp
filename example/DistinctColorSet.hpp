#ifndef TRAJECTORY_DETECTION_DISTINCTCOLORSET_H
#define TRAJECTORY_DETECTION_DISTINCTCOLORSET_H

#include <string>
#include <array>

class DistinctColorSet {
    int _index = 0;

    const std::array<const std::string, 20> _colors {
            "rgb(255, 179, 0)",
            "rgb(128, 62, 117)",
            "rgb(255, 104, 0)",
            "rgb(166, 189, 215)",
            "rgb(193, 0, 32)",
            "rgb(206, 162, 98)",
            "rgb(129, 112, 102)",
            "rgb(0, 125, 52)",
            "rgb(246, 118, 142)",
            "rgb(0, 83, 138)",
            "rgb(255, 122, 92)",
            "rgb(83, 55, 122)",
            "rgb(255, 142, 0)",
            "rgb(179, 40, 81)",
            "rgb(244, 200, 0)",
            "rgb(127, 24, 13)",
            "rgb(147, 170, 0)",
            "rgb(89, 51, 21)",
            "rgb(241, 58, 19)",
            "rgb(35, 44, 22)",
    };
public:
    std::string next() {
        return _colors[(_index++) % _colors.size()];
    }
};


#endif //TRAJECTORY_DETECTION_DISTINCTCOLORSET_H

#include "DistinctColorSet.h"

#include <iterator>

std::string DistinctColorSet::next() {
    return _colors[(_index++) % _colors.size()];
}

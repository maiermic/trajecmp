#ifndef COLOR_HPP
#define COLOR_HPP

struct rgb {
    int red, green, blue;
};

namespace color_code {
    const rgb green{0, 255, 0};
    const rgb red{255, 0, 0};
    const rgb yellow{255, 255, 0};
}

#endif //COLOR_HPP

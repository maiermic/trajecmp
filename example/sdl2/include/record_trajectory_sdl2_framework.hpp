#ifndef TRAJECMP_RECORD_TRAJECTORY_SDL2_FRAMEWORK_HPP
#define TRAJECMP_RECORD_TRAJECTORY_SDL2_FRAMEWORK_HPP

#include <sdl2_framework.hpp>
#include <model.hpp>
#include <pattern_matching.hpp>
#include <draw.hpp>

class record_trajectory_sdl2_framework : public sdl2_framework {
    bool _is_rerender = true;
    bool _is_recording_trajectory = false;
    model::trajectory _trajectory;
public:
    bool is_rerender() const;

    void is_rerender(bool is_rerender);

    virtual void handle_input_trajectory(model::trajectory input)= 0;

    void display() override;

    void handle_event(const SDL_Event &event) override;

    void transform_for_visualization(model::trajectory &trajectory);

    model::point get_visualization_center();
};

#endif //TRAJECMP_RECORD_TRAJECTORY_SDL2_FRAMEWORK_HPP

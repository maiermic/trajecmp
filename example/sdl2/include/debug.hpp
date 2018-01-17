#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <string>
#include <rxcpp/rx.hpp>

#include "../../logging.hpp"
#include "model.hpp"

inline auto debug(std::string description) {
    return [=](auto &&observable) {
        using Value = typename rxcpp::util::value_type_from<decltype(observable)>::type;
        return observable.tap([=](Value value) {
            logging::is_logging = true;
            LOG_TEXT(description << ": " << value);
            logging::is_logging = false;
        });
    };
}

#endif //DEBUG_HPP

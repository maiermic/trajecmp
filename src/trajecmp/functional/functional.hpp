#ifndef TRAJECMP_FUNCTIONAL_FUNCTIONAL_HPP
#define TRAJECMP_FUNCTIONAL_FUNCTIONAL_HPP

#include <functional>

namespace trajecmp { namespace functional {
    namespace detail {

        struct compose {
            template<typename L, typename R>
            auto operator()(L l, R r) const {
                return [=](auto arg) {
                    return l(r(arg));
                };
            }

            template<typename H, typename G, typename... Fs>
            auto operator()(H h, G g, Fs... f) const {
                return compose::operator()(h, compose::operator()(g, f...));
            }
        };

        struct pipe {
            template<typename L, typename R>
            auto operator()(L l, R r) const {
                return [=](auto arg) {
                    return r(l(arg));
                };
            }

            template<typename H, typename G, typename... Fs>
            auto operator()(H h, G g, Fs... f) const {
                return pipe::operator()(pipe::operator()(h, g), f...);
            }
        };

        struct mem_fn {
            template<typename T>
            auto operator()(T t) const {
                return std::mem_fn(t);
            }
        };

        template<typename F>
        auto call(F f) {
            return f();
        }

    } // namespace detail


    template<typename F>
    auto call_arguments_on_function_call(F f) {
        return [=](auto &&... args) {
            return [=]() {
                return f(detail::call(args)...);
            };
        };
    }

    template<typename M, typename F>
    auto map_function_arguments(M m, F f) {
        return [=](auto &&... args) {
            return f(m(args)...);
        };
    }

    const detail::compose compose;
    const detail::pipe pipe;
    const detail::mem_fn mem_fn;
    const auto nested_mem_fn = map_function_arguments(mem_fn, pipe);

    template<typename L, typename R>
    auto operator|(L l, R r) {
        return pipe(l, r);
    }

}} // namespace trajecmp::functional

#endif //TRAJECMP_FUNCTIONAL_FUNCTIONAL_HPP

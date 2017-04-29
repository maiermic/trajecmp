#ifndef TRAJECMP_UTIL_SUBSCRIBE_WITH_LATEST_FROM_HPP
#define TRAJECMP_UTIL_SUBSCRIBE_WITH_LATEST_FROM_HPP


namespace trajecmp { namespace util {

    template<typename Fn, typename... Observables>
    auto subscribe_with_latest_from(Fn f, Observables... observables) {
        return [=](auto &&source) {
            return source
                    .with_latest_from(
                            [=](auto &&...args) {
                                f(args...);
                                return 0; // dummy value
                            },
                            observables...
                    )
                    .subscribe([](auto _) {});
        };
    }

}} // namespace trajecmp::util


#endif //TRAJECMP_UTIL_SUBSCRIBE_WITH_LATEST_FROM_HPP

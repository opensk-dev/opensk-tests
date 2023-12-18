#ifndef SK_TESTS_COMMON_DECORATORS_HPP
#define SK_TESTS_COMMON_DECORATORS_HPP

#include <chrono>
#include <functional>
#include <future>

namespace sk::test {

using Delegate = std::function<void()>;
using Decorator = std::function<void(const Delegate&)>;

Delegate decorate(const Delegate& delegate, const Decorator& decorator) {
    return [=]() {
        decorator(delegate);
    };
}

template<typename... Decorators>
Delegate decorate(
        const Delegate& delegate,
        const Decorator& decorator1, const Decorator& decorator2,
        const Decorators&... other_decorators) {
    return decorate(decorate(delegate, decorator1), decorator2, other_decorators...);
}

template<typename... Args>
Decorator provide(const auto& decorator, Args&&... args) {
    return [decorator, ... args = std::forward<Args>(args)](const Delegate& delegate) mutable {
        decorator(delegate, std::forward<Args>(args)...);
    };
}

void require_nothrow(const Delegate& delegate) {
    BOOST_REQUIRE_NO_THROW(delegate(););
}

void timer(const Delegate& delegate, const std::string& name) {
    auto begin = std::chrono::system_clock::now();
    std::invoke(delegate);
    auto end = std::chrono::system_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << std::format("{} time begin: {}\n", name, begin);
    std::cout << std::format("{} time end: {}\n", name, end);
    std::cout << std::format("total {} time: {}us\n", name, total_time);
}

void run_with_timeout(const Delegate& delegate, std::chrono::milliseconds timeout) {
    auto future = std::async(std::launch::async, delegate);
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout) {
        auto error_message = std::format("Timeout: the test waiting time is more than {}ms", timeout.count());
        BOOST_TEST_FAIL(error_message);
        throw std::runtime_error(error_message);
    }
}

}// namespace sk::test

#endif// SK_TESTS_COMMON_DECORATORS_HPP

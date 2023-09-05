# include <core/runtime_arena.hpp>

# include <test/common.hpp>

# include <array>
# include <format>
# include <iostream>
# include <thread>
# include <vector>

BOOST_AUTO_TEST_SUITE(runtime_arena_test_suite)

class TestTask : public sk::Task {
public:
    TestTask(int i, bool& done)
        : i_(i), done_(done) {
    }

    void execute() override {
        std::cout << std::format("task {} execute\n", i_);
        done_ = true;
    }

private:
    int i_ = 0;
    bool& done_;
};

class TasksGroup {
public:
    TasksGroup() {
        tasks_.clear();
        tasks_.reserve(tasks_count_);
        for (int i = 0; i < tasks_count_; ++i) {
            tasks_.emplace_back(i, tasks_done_[i]);
        }
    };

    void run(sk::RuntimeArena& arena) {
        for (auto& task : tasks_) {
            arena.push_task(&task);
        }
    }

    sk::test::Decorator require_tasks_done() {
        return [this](const sk::test::Delegate& delegate) {
            tasks_done_.fill(false);
            std::invoke(delegate);
            check_that_all_tasks_done();
        };
    };

private:
    static constexpr int tasks_count_ = 10;
    std::array<bool, tasks_count_> tasks_done_{};
    std::vector<TestTask> tasks_{};

    bool check_that_all_tasks_done() {
        bool complete = true;
        for (auto& done : tasks_done_) {
            complete = complete && done;
        }
        return complete;
    };
};

class TestDefaultBehaviour : public sk::test::TestSuite {
public:
    void run_tests() override {
        test_case("unmanaged mode", test_unmanaged_mode());
        test_case("managed mode", test_managed_mode());
        test_case("captured mode", test_captured_mode());
    };

private:
    TasksGroup tasks_group_{};

    sk::test::Delegate test_unmanaged_mode_ =
        [this]() {
            sk::RuntimeArena arena{};
            tasks_group_.run(arena);
        };

    [[nodiscard]] sk::test::Delegate test_unmanaged_mode() {
        using namespace std::chrono_literals;
        return sk::test::decorate(
            test_unmanaged_mode_,
            sk::test::require_nothrow,
            sk::test::provide(sk::test::run_with_timeout, 10ms),
            tasks_group_.require_tasks_done(),
            sk::test::provide(sk::test::timer, std::string("test_unmanaged_mode")));
    }

    sk::test::Delegate test_managed_mode_ =
        [this]() {
            sk::RuntimeArena arena{sk::RuntimeArena::Managed{}};
            arena.start();
            tasks_group_.run(arena);
            arena.stop();
        };

    [[nodiscard]] sk::test::Delegate test_managed_mode() {
        using namespace std::chrono_literals;
        return sk::test::decorate(
            test_managed_mode_,
            sk::test::require_nothrow,
            sk::test::provide(sk::test::run_with_timeout, 10ms),
            tasks_group_.require_tasks_done(),
            sk::test::provide(sk::test::timer, std::string("test_managed_mode")));
    }

    sk::test::Delegate test_captured_mode_ =
        [this]() {
            sk::RuntimeArena arena{sk::RuntimeArena::Managed{}};
            std::thread controller(
                [&]() {
                    tasks_group_.run(arena);
                    arena.stop();
                });
            arena.capture();
            controller.join();
        };

    [[nodiscard]] sk::test::Delegate test_captured_mode() {
        using namespace std::chrono_literals;
        return sk::test::decorate(
            test_captured_mode_,
            sk::test::require_nothrow,
            sk::test::provide(sk::test::run_with_timeout, 10ms),
            tasks_group_.require_tasks_done(),
            sk::test::provide(sk::test::timer, std::string("test_captured_mode")));
    }
};

BOOST_AUTO_TEST_CASE(test_default_behaviour) {
    TestDefaultBehaviour test_suite{};
    test_suite.run_tests();
}

class TestManagedMode : public sk::test::TestSuite {
public:
    void run_tests() override {
        test_case("managed mode: prev queue access", test_prev_queue_access());
    }

private:
    TasksGroup tasks_group_{};

    sk::test::Delegate test_prev_queue_access_ =
        [this]() {
            sk::RuntimeArena arena{sk::RuntimeArena::Managed{}};
            tasks_group_.run(arena);
            arena.start();
        };

    [[nodiscard]] sk::test::Delegate test_prev_queue_access() {
        using namespace std::chrono_literals;
        return sk::test::decorate(
            test_prev_queue_access_,
            sk::test::require_nothrow,
            sk::test::provide(sk::test::run_with_timeout, 10ms),
            tasks_group_.require_tasks_done(),
            sk::test::provide(sk::test::timer, std::string("test_prev_queue_access")));
    }
};

BOOST_AUTO_TEST_CASE(test_managed_mode) {
    TestManagedMode test_suite;
    test_suite.run_tests();
}

class TestCapturedMode : public sk::test::TestSuite {
public:
    void run_tests() override {
        test_case("captured mode: prev queue access", test_prev_queue_access());
        test_case("captured mode: test inner stop", test_inner_stop());
    }

private:
    TasksGroup tasks_group_{};

    sk::test::Delegate test_prev_queue_access_ =
        [this]() {
            sk::RuntimeArena arena{sk::RuntimeArena::Managed{}};
            tasks_group_.run(arena);
            arena.stop_in_future();
            arena.capture();
        };

    [[nodiscard]] sk::test::Delegate test_prev_queue_access() {
        using namespace std::chrono_literals;
        return sk::test::decorate(
            test_prev_queue_access_,
            sk::test::require_nothrow,
            sk::test::provide(sk::test::run_with_timeout, 10ms),
            tasks_group_.require_tasks_done(),
            sk::test::provide(sk::test::timer, std::string("test_prev_queue_access")));
    }

    class InnerStopTask : public sk::Task {
    public:
        explicit InnerStopTask(sk::RuntimeArena& arena)
            : arena_(arena) {
        }

        void execute() override {
            std::cout << "Task: execute in\n";
            arena_.stop();
            std::cout << "Task: execute out\n";
            valid_ = true;
        }

        [[nodiscard]] bool is_valid() const {
            return valid_;
        }

    private:
        bool valid_ = false;
        sk::RuntimeArena& arena_;
    };

    sk::test::Delegate test_inner_stop_ =
        []() {
            sk::RuntimeArena arena{sk::RuntimeArena::Managed{}};
            InnerStopTask task(arena);
            arena.push_task(&task);
            arena.capture();
            BOOST_REQUIRE_EQUAL(task.is_valid(), true);
        };

    [[nodiscard]] sk::test::Delegate test_inner_stop() {
        using namespace std::chrono_literals;
        return sk::test::decorate(
            test_inner_stop_,
            sk::test::require_nothrow,
            sk::test::provide(sk::test::run_with_timeout, 10ms),
            sk::test::provide(sk::test::timer, std::string("test_inner_stop")));
    }
};

BOOST_AUTO_TEST_CASE(test_captured_mode) {
    TestCapturedMode test_suite;
    test_suite.run_tests();
}

BOOST_AUTO_TEST_SUITE_END()
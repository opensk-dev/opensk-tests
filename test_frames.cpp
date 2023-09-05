# include <core/frame.hpp>

# include <test/common.hpp>

# include <vector>
# include <stack>

BOOST_AUTO_TEST_SUITE(frames_test_suite)

class TestFrame : public sk::Frame {
public:
    void initialize() override {
        Frame::initialize();
        history_.push_back(ActionsEnum::initialize);
        std::cout << "initialize\n";
    }

    void finalize() override {
        Frame::finalize();
        std::cout << "finalize\n";
        history_.push_back(ActionsEnum::finalize);
    }

    void on_enable() override {
        Frame::on_enable();
        history_.push_back(ActionsEnum::enable);
        std::cout << "enable\n";
    }

    void on_disable() override {
        Frame::on_disable();
        history_.push_back(ActionsEnum::disable);
        std::cout << "disable\n";
    }

    [[nodiscard]] bool is_valid() const {
        std::stack<StatesEnum> state;
        for (auto& action : history_) {
            switch (action) {
                case ActionsEnum::initialize: {
                    if (state.empty()) {
                        state.push(StatesEnum::initialized);
                    } else {
                        return false;
                    }
                    break;
                }
                case ActionsEnum::enable: {
                    if (state.top() == StatesEnum::initialized) {
                        state.push(StatesEnum::enabled);
                    } else {
                        return false;
                    }
                    break;
                }
                case ActionsEnum::disable: {
                    if (state.top() == StatesEnum::enabled) {
                        state.pop();
                    } else {
                        return false;
                    }
                    break;
                }
                case ActionsEnum::finalize: {
                    if (state.top() == StatesEnum::initialized) {
                        state.pop();
                    } else {
                        return false;
                    }
                    break;
                }
            }
        }
        return true;
    }

private:
    enum class StatesEnum {
        initialized = 0,
        enabled = 1,
    };

    enum class ActionsEnum {
        initialize = 0,
        enable = 1,
        disable = 2,
        finalize = 3,
    };

    std::vector<ActionsEnum> history_{};
};

BOOST_AUTO_TEST_CASE(test_lifecycle) {
    auto test = []() {
        TestFrame frame;
        frame.initialize();
        frame.on_enable();
        frame.on_disable();
        frame.finalize();
        BOOST_REQUIRE_EQUAL(frame.is_valid(), true);
    };

    auto delegate = sk::test::decorate(
        test,
        sk::test::require_nothrow
    );

    std::invoke(delegate);
}

BOOST_AUTO_TEST_CASE(test_frame_holder) {
    auto test = []() {
        TestFrame frame1;
        TestFrame frame2;
        frame1.initialize();
        frame2.initialize();
        {
            sk::FrameHolder frame_holder;
            frame_holder = &frame1;
            frame_holder = &frame2;
            frame_holder = &frame1;
        }
        frame1.finalize();
        frame2.finalize();
        BOOST_REQUIRE_EQUAL(frame1.is_valid(), true);
        BOOST_REQUIRE_EQUAL(frame2.is_valid(), true);
    };

    auto delegate = sk::test::decorate(
        test,
        sk::test::require_nothrow
    );

    std::invoke(delegate);
}

BOOST_AUTO_TEST_SUITE_END()
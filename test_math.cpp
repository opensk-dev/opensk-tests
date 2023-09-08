# include <math/rect.hpp>

# include <test/common.hpp>

BOOST_AUTO_TEST_SUITE(sk_math_test_suite)

class TestRect : public sk::test::TestSuite {
public:
    void run_tests() override {
        test_case("point_point", test_point_point_);
        test_case("point_size", test_point_size_);
    }

private:
    template<class RectType>
    void check(RectType rect, int x0, int y0, int w, int h) {
        int x1 = x0 + w;
        int y1 = y0 + h;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x0()), x0);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y0()), y0);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x1()), x1);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y1()), y1);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.width()), w);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.height()), h);

        rect.x0() = rect.x0() - 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x0()), x0 - 1);
        rect.y0() = rect.y0() - 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y0()), y0 - 1);
        rect.x1() = rect.x1() + 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x1()), x1 + 1);
        rect.y1() = rect.y1() + 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y1()), y1 + 1);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.width()), w + 2);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.height()), h + 2);

        rect.x0() = rect.x0() + 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x0()), x0);
        rect.y0() = rect.y0() + 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y0()), y0);
        rect.x1() = rect.x1() - 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x1()), x1);
        rect.y1() = rect.y1() - 1;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y1()), y1);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.width()), w);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.height()), h);

        rect.x0() = rect.x0() + 2;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x0()), x0 + 2);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.width()), w - 2);
        rect.x1() = rect.x1() + 2;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.x1()), x1 + 2);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.width()), w);

        rect.y0() = rect.y0() + 3;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y0()), y0 + 3);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.height()), h - 3);
        rect.y1() = rect.y1() + 3;
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.y1()), y1 + 3);
        BOOST_REQUIRE_EQUAL(static_cast<int>(rect.height()), h);
    }

    sk::test::Delegate test_point_point_ =
        [this]() {
            using namespace sk::math;
            {
                Rect<RectType::point_point> rect({2, 1}, {4, 2});
                check(rect, 2, 1, 2, 1);
            }
            {
                Rect<RectType::point_point> rect({-4, 3}, {-2, 5});
                check(rect, -4, 3, 2, 2);
            }
            {
                Rect<RectType::point_point> rect({-4, -3}, {-2, 0});
                check(rect, -4, -3, 2, 3);
            }
            {
                Rect<RectType::point_point> rect({0, -1}, {4, 0});
                check(rect, 0, -1, 4, 1);
            }
            {
                Rect<RectType::point_point> rect({-1, -5}, {1, -2});
                check(rect, -1, -5, 2, 3);
            }
            {
                Rect<RectType::point_point> rect({-2, -1}, {1, 3});
                check(rect, -2, -1, 3, 4);
            }
            {
                Rect<RectType::point_point> rect({5, -1}, {8, 2});
                check(rect, 5, -1, 3, 3);
            }
        };

    sk::test::Delegate test_point_size_ =
        [this]() {
            using namespace sk::math;
            {
                Rect<RectType::point_size> rect({2, 1}, {2, 1});
                check(rect, 2, 1, 2, 1);
            }
            {
                Rect<RectType::point_size> rect({-4, 3}, {2, 2});
                check(rect, -4, 3, 2, 2);
            }
            {
                Rect<RectType::point_size> rect({-4, -3}, {2, 3});
                check(rect, -4, -3, 2, 3);
            }
            {
                Rect<RectType::point_size> rect({0, -1}, {4, 1});
                check(rect, 0, -1, 4, 1);
            }
            {
                Rect<RectType::point_size> rect({-1, -5}, {2, 3});
                check(rect, -1, -5, 2, 3);
            }
            {
                Rect<RectType::point_size> rect({-2, -1}, {3, 4});
                check(rect, -2, -1, 3, 4);
            }
            {
                Rect<RectType::point_size> rect({5, -1}, {3, 3});
                check(rect, 5, -1, 3, 3);
            }
        };
};

BOOST_AUTO_TEST_CASE(test_rect) {
    TestRect test_suite;
    test_suite.run_tests();
}

BOOST_AUTO_TEST_SUITE_END()
#include "../src/tool.hpp"

#include <gtest/gtest.h>

TEST(observableValue, observableValue) {
    using namespace minecpp;
    ObservableValue<int> a;
    ObservableValue<int> b;
    
    bool isObservered = false;
    Observer observer{[&isObservered](auto a) { isObservered = true; }, a};
    EXPECT_EQ(isObservered, false);
    a = 1;
    EXPECT_EQ(isObservered, true);

    auto c = makeReactiveValue([](int a, int b) { return a + b; }, a, b);
    b = 2;
    EXPECT_EQ(c.get(), 3);
    a = 3;
    EXPECT_EQ(c.get(), 5);

    auto copy = c;
    b = 5;
    EXPECT_EQ(copy.get(), 8);
    EXPECT_EQ(c.get(), 8);

    auto move = std::move(c);
    b = 6;
    EXPECT_EQ(move.get(), 9);
    EXPECT_NE(c.get(), 9);

    auto aa = a;
    aa = 1;

    EXPECT_EQ(move.get(), 9);
}

import <stl>;
import <fmt>;
import tool;
#include <snitch_all.hpp>


TEST_CASE("observableValue", "[observableValue]") {
    using namespace minecpp;
    ObservableValue<int> a;
    ObservableValue<int> b;
    
    bool observed = false;
    Observer observer{[&observed](auto a) { observed = true; }, a};
    REQUIRE(observed == false);
    a = 1;
    REQUIRE(observed == true);

    auto c = makeReactiveValue([](int a, int b) { return a + b; }, a, b);
    b = 2;
    REQUIRE(c.get() == 3);
    a = 3;
    REQUIRE(c.get() == 5);

    auto copy = c;
    b = 5;
    REQUIRE(copy.get() == 8);
    REQUIRE(c.get() == 8);

    auto move = std::move(c);
    b = 6;
    REQUIRE(move.get() == 9);
    REQUIRE(c.get() != 9);

    auto aa = a;
    aa = 1;

    REQUIRE(move.get() == 9);
}

TEST_CASE("observableValue", "[reactiveBinder]") {
    using namespace minecpp;
    ObservableValue<int> a1;
    ObservableValue<int> a2;
    ObservableValue<int> b;

    ObservableValue<int> c;
    auto binder1 = makeReactiveBinder([](int a, int b) { return a + b; }, c, a1, b);
    auto binder2 = [&]{return makeReactiveBinder([](int a, int b) { return a + b; }, c, a2, b);}();
    c = 123;
    REQUIRE(c.get() == 123);
    
    b = 2;
    a1 = 1;
    REQUIRE(c.get() == 3);
    a2 = 3;
    REQUIRE(c.get() == 5);

    c = 123;
    REQUIRE(c.get() == 123);

    auto copy = c;
    b = 5;
    a1 = 5;
    REQUIRE(copy.get() == 123);
    REQUIRE(c.get() == 10);
}

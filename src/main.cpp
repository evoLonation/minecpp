// #include "example/light_caster.hpp"
// #include "example/multi_light.hpp"
// #include "example/model.hpp"
#include <fmt/format.h>
#include "fmt/core.h"
#include "tool.hpp"

using namespace minecpp;

int main()
{
   ObservableValue<int> a;
   ObservableValue<int> b;
   Observer observer {[] (auto a) {
      fmt::println("giao, {}", a);
   }, a};
   auto c = makeReactiveValue([](int a, int b){return a + b;}, a, b);
   fmt::println("{}", a.get());
   fmt::println("{}", b.get());
   a = 1;
   b = 2;
   

   fmt::println("{}", a.get());
   fmt::println("{}", b.get());
   fmt::println("{}", c.get());
   auto d = std::move(c);
   a = 3; b = 4;
   fmt::println("{}", d.get());
   fmt::println("{}", c.get());
   auto aa = a;
   a = 5;
   aa = 1;
   fmt::println("{}", d.get());
   // return light_caster::run();
   // return multi_light::run();
   // return model::run();
}
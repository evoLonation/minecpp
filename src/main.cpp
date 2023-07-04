// #include "example/material.hpp"
// #include "example/3d_lighting_2.hpp"
// #include "example/imgui.hpp"
// #include "example/2d_transform.hpp"
// #include "example/3d_lighting.hpp"
// #include "example/3d_transform.hpp"
// #include "example/3d_transform_template.hpp"
#include "resource.hpp"
#include "example/lighting_mappings.hpp"
#include "example/light_caster.hpp"
#include "tool.hpp"

int main()
{  
   using namespace minecpp;
   BaseValue a = 1;
   BaseValue b = 2.0f;
   ReactiveValue c {[](int a, float b) -> int {
      fmt::println("change to {} !", a+b);
      return a + b;
   }, a, b};
   a = 2;
   b = 6.0f;
   // return material::run();
   // return lighting_3d_2::run();
   // return imgui::run();
   return light_caster::run();
   // return lighting_mappings::run();
}
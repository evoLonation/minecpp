// #include "example/material.hpp"
// #include "example/3d_lighting_2.hpp"
// #include "example/imgui.hpp"
// #include "example/2d_transform.hpp"
// #include "example/3d_lighting.hpp"
// #include "example/3d_transform.hpp"
// #include "example/3d_transform_template.hpp"
// #include "resource.hpp"
// #include "example/lighting_mappings.hpp"
// #include "example/light_caster.hpp"
#include "tool.hpp"
#include "fmt/format.h"

int main()
{  
   using namespace minecpp;
   AssignObservable asdf = 10;
   AssignObservable a = asdf.get();
   ManualObservable b = asdf.get();
   ReactiveValue c {[](int a, int b){
      fmt::println("a + b = {}", a + b);
      return a + b;
   }, a, b};
   a = 2;
   b.val() = 3;
   b.notice();
   fmt::println("now b: {}", *&b);


   // return material::run();
   // return lighting_3d_2::run();
   // return imgui::run();
   // return light_caster::run();
   // return lighting_mappings::run();
}
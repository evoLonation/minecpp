// #include "example/material.hpp"
// #include "example/3d_lighting_2.hpp"
// #include "example/imgui.hpp"
// #include "example/2d_transform.hpp"
// #include "example/3d_lighting.hpp"
// #include "example/3d_transform.hpp"
// #include "example/3d_transform_template.hpp"
// #include "resource.hpp"
// #include "example/lighting_mappings.hpp"
#include "example/light_caster.hpp"
#include "example/multi_light.hpp"
using namespace minecpp;

int main()
{  
   IdContainer<int&> a;
   int num1 = 111;
   int num2 = 222;
   int id = a.add(num1);
   int id2 = a.add(num2);
   num1 = 333;
   a.forEach([](auto num){
      fmt::println("{}", num);
   });
   a.remove(id2);
   a.forEach([](auto num){
      fmt::println("{}", num);
   });
   a.remove(id);

   
   // return material::run();
   // return lighting_3d_2::run();
   // return imgui::run();
   // return light_caster::run();
   return multi_light::run();
   // return lighting_mappings::run();
}
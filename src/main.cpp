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

class A{
   int a;
   int b[10];
};
class B{
   A& a;
};
class C{
   A a;
};

int main()
{  
   // return material::run();
   // return lighting_3d_2::run();
   // return imgui::run();
   return light_caster::run();
   return lighting_mappings::run();
}
// #include "example/light_caster.hpp"
#include "example/multi_light.hpp"
#include "example/model.hpp"
#include <fmt/format.h>
#include <optional>

using namespace minecpp;


int main()
{
   auto a = 1;
   auto& ar = a;
   auto op = std::optional(ar);
   // a.a = 3;
   
   // return light_caster::run();
   while(true){
      multi_light::run();
   }
   return model::run();
}
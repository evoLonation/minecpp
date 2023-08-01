#include "example/multi_light.hpp"
#include "example/model.hpp"
// #include "example/auxiliary.hpp"
#include "tool.hpp"
#include <iostream>
#include <type_traits>



using namespace minecpp;

int main()
{
   multi_light::run();
   model::run();
   return 0;
   // return auxiliary::run();
   // return model::run();
}
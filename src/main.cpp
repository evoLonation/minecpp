#include "example/multi_light.hpp"
#include "example/model.hpp"
#include "example/auxiliary.hpp"



using namespace minecpp;

int main()
{
   auxiliary::run();
   multi_light::run();
   model::run();
   return 0;
   // return model::run();
}
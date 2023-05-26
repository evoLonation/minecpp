#include "example/2d_transform.hpp"
#include "render/render.hpp"
#include <fmt/format.h>

int main(){
   if(transform2d::run()){
      printf("success run\n");
   }
   using namespace std;
   cout<<fmt::format("{}\n", 1); // format，输出完整的3.14159265358979
   return 0;   
}
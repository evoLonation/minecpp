#include "example/light_caster.hpp"
#include "example/multi_light.hpp"
#include "example/model.hpp"

using namespace minecpp;

int main()
{
   // return light_caster::run();
   return multi_light::run();
   return model::run();
   ChangeableObservable<glm::vec3> color = glm::vec3();
   ChangeableObservable<glm::vec3> direction = glm::vec3();
}
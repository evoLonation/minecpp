#ifndef _MINECPP_LIGHT_H_
#define _MINECPP_LIGHT_H_

#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace minecpp
{
   
void getAttenuation(unsigned distance, float& constant, float& linear, float& quadratic){
   std::tuple<float, float, float> ret;
   if(distance <= 7){
      ret = {1.0, 0.7, 1.8};
   }else if(distance <= 13){
      ret = {1.0, 0.35, 0.44};
   }else if(distance <= 20){
      ret = {1.0, 0.22, 0.20};
   }else if(distance <= 32){
      ret = {1.0, 0.14, 0.07};
   }else if(distance <= 50){
      ret = {1.0, 0.09, 0.032};
   }else if(distance <= 65){
      ret = {1.0, 0.07, 0.017};
   }else if(distance <= 100){
      ret = {1.0, 0.045, 0.0075};
   }else if(distance <= 160){
      ret = {1.0, 0.027, 0.0028};
   }else if(distance <= 200){
      ret = {1.0, 0.022, 0.0019};
   }else if(distance <= 325){
      ret = {1.0, 0.014, 0.0007};
   }else if(distance <= 600){
      ret = {1.0, 0.007, 0.0002};
   }else if(distance <= 3250){
      ret = {1.0, 0.0014, 0.000007};
   }
   constant = std::get<0>(ret);
   linear = std::get<1>(ret);
   quadratic = std::get<2>(ret);
}

struct Attenuation {
   float linear;
   float quadratic;
   float constant;
};

struct LightMaterial{
   glm::vec3 diffuse;
   glm::vec3 ambient;
   glm::vec3 specular;
};

} // namespace minecpp


#endif // _MINECPP_LIGHT_H_
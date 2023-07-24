#ifndef _MINECPP_CONTROLLER_H_
#define _MINECPP_CONTROLLER_H_

#include "light.hpp"
#include "gui.hpp"

namespace minecpp
{

class DirectionalLightUIController: UnCopyMoveable{
private:
   DirectionalLightExample* light;
public:
   DirectionalLightUIController(): light(nullptr){}
   DirectionalLightUIController(DirectionalLightExample& light): light(&light){}
   DirectionalLightUIController& operator=(DirectionalLightExample& light){
      this->light = &light;
      return *this;
   }
   void showControllerPanel(){
      if(light == nullptr){
         ImGui::Text("not yet bound to any lights");
         return;
      }
      ImGui::ColorEdit3("light color", glm::value_ptr(light->color.val()));
      light->color.mayNotice();
      slider("light direction", light->direction);
   }
};
class PointLightUIController: UnCopyMoveable{
private:
   PointLightExample* light;
public:
   PointLightUIController(): light(nullptr){}
   PointLightUIController(PointLightExample& light): light(&light){}
   PointLightUIController& operator=(PointLightExample& light){
      this->light = &light;
      return *this;
   }
   void showControllerPanel(){
      if(light == nullptr){
         ImGui::Text("not yet bound to any lights");
         return;
      }
      ImGui::ColorEdit3("light color", glm::value_ptr(light->color.val()));
      light->color.mayNotice();
      slider("light position", light->position);
      slider("light max distance", light->distance, 0.0f, 100.0f);
   }
};
class SpotLightUIController: UnCopyMoveable{
private:
   SpotLightExample* light;
public:
   SpotLightUIController(): light(nullptr){}
   SpotLightUIController(SpotLightExample& light): light(&light){}
   SpotLightUIController& operator=(SpotLightExample& light){
      this->light = &light;
      return *this;
   }
   void showControllerPanel(){
      if(light == nullptr){
         ImGui::Text("not yet bound to any lights");
         return;
      }
      ImGui::ColorEdit3("light color", glm::value_ptr(light->color.val()));
      light->color.mayNotice();
      slider("light position", light->position);
      slider("light direction", light->direction);
      slider("light max distance", light->distance, 0.0f, 100.0f);
      slider("light inner cut off degree", light->innerCutOffDegree, 0.0f, 90.0f);
      slider("light outer cut off degree", light->outerCutOffDegree, 0.0f, 90.0f);
   }
};
} // namespace minecpp


#endif // _MINECPP_CONTROLLER_H_
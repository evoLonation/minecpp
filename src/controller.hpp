#ifndef _MINECPP_CONTROLLER_H_
#define _MINECPP_CONTROLLER_H_

#include "light.hpp"
#include "gui.hpp"

namespace minecpp
{

class DirectionalLightUIController{
private:
   DirectionalLightData* light;
public:
   DirectionalLightUIController(): light(nullptr){}
   DirectionalLightUIController(DirectionalLightData& light): light(&light){}
   DirectionalLightUIController& operator=(DirectionalLightData& light){
      this->light = &light;
      return *this;
   }
   void showControllerPanel(){
      if(light == nullptr){
         ImGui::Text("not yet bound to any lights");
         return;
      }
      ImGui::ColorEdit3("light color", glm::value_ptr(light->color.get()));
      light->color.mayNotify();
      slider("light direction", light->direction);
   }
   // deleted copy semantic
   DirectionalLightUIController& operator=(const DirectionalLightUIController&) = delete;
   DirectionalLightUIController(const DirectionalLightUIController&) = delete;
   // deleted move semantic
   DirectionalLightUIController& operator=(DirectionalLightUIController&&) = delete;
   DirectionalLightUIController(DirectionalLightUIController&&) = delete;
   
};
class PointLightUIController{
private:
   PointLightData* light;
public:
   PointLightUIController(): light(nullptr){}
   PointLightUIController(PointLightData& light): light(&light){}
   PointLightUIController& operator=(PointLightData& light){
      this->light = &light;
      return *this;
   }
   void reset(){
      this->light = nullptr;
   }
   void showControllerPanel(){
      if(light == nullptr){
         ImGui::Text("not yet bound to any lights");
         return;
      }
      ImGui::ColorEdit3("light color", glm::value_ptr(light->color.get()));
      light->color.mayNotify();
      slider("light position", light->position);
      slider("light max distance", light->distance, 0.0f, 100.0f);
   }
   // deleted copy semantic
   PointLightUIController& operator=(const PointLightUIController&) = delete;
   PointLightUIController(const PointLightUIController&) = delete;
   // deleted move semantic
   PointLightUIController& operator=(PointLightUIController&&) = delete;
   PointLightUIController(PointLightUIController&&) = delete;
   
};
class SpotLightUIController{
private:
   SpotLightData* light;
public:
   SpotLightUIController(): light(nullptr){}
   SpotLightUIController(SpotLightData& light): light(&light){}
   SpotLightUIController& operator=(SpotLightData& light){
      this->light = &light;
      return *this;
   }
   void reset(){
      light = nullptr;
   }
   void showControllerPanel(){
      if(light == nullptr){
         ImGui::Text("not yet bound to any lights");
         return;
      }
      ImGui::ColorEdit3("light color", glm::value_ptr(light->color.get()));
      light->color.mayNotify();
      slider("light position", light->position);
      slider("light direction", light->direction);
      slider("light max distance", light->distance, 0.0f, 100.0f);
      slider("light inner cut off degree", light->innerCutOffDegree, 0.0f, 90.0f);
      slider("light outer cut off degree", light->outerCutOffDegree, 0.0f, 90.0f);
   }
   // deleted copy semantic
   SpotLightUIController& operator=(const SpotLightUIController&) = delete;
   SpotLightUIController(const SpotLightUIController&) = delete;
   // deleted move semantic
   SpotLightUIController& operator=(SpotLightUIController&&) = delete;
   SpotLightUIController(SpotLightUIController&&) = delete;
   
};
} // namespace minecpp


#endif // _MINECPP_CONTROLLER_H_
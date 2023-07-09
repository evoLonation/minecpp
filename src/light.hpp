#ifndef _MINECPP_LIGHT_H_
#define _MINECPP_LIGHT_H_

#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "tool.hpp"
#include "resource.hpp"
#include "model.hpp"

namespace minecpp
{
   
struct Attenuation {
   float constant;
   float linear;
   float quadratic;
};

Attenuation computeAttenuation(int distance){
   Attenuation ret;
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
   return ret;
}

struct ReactiveAttenuation: public ReactiveValue<Attenuation, int>{
   ReactiveAttenuation(const ObservableValue<int>& distance): ReactiveValue<Attenuation, int>(computeAttenuation, distance){};
};


struct LightMaterial{
   glm::vec3 diffuse;
   glm::vec3 ambient;
   glm::vec3 specular;
};

LightMaterial computeLightMaterial(const glm::vec3& color){
   auto lightDiffuse = color * glm::vec3(0.5f);
   auto lightAmbient = color * glm::vec3(0.2f);
   auto lightSpecular = color;
   return LightMaterial {
      lightDiffuse,
      lightAmbient,
      lightSpecular
   };
}

struct ReactiveMaterial: public ReactiveValue<LightMaterial, glm::vec3>{
   ReactiveMaterial(const ObservableValue<glm::vec3>& color): ReactiveValue<LightMaterial, glm::vec3>(computeLightMaterial, color){};
};

struct ReactiveNormalize: public ReactiveValue<glm::vec3, glm::vec3>{
   ReactiveNormalize(const ObservableValue<glm::vec3>& vec): ReactiveValue<glm::vec3, glm::vec3>([](const glm::vec3& vec){return glm::normalize(vec);}, vec){};
};

struct ReactiveCos: public ReactiveValue<float, float>{
   ReactiveCos(const ObservableValue<float>& value): ReactiveValue<float, float>([](float value){return glm::cos(value);}, value){};
};

inline AssignObservable<glm::vec3> defaultDirectionalLightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
inline AssignObservable<glm::vec3> defaultDirectionalLightDirection { glm::vec3(0.0f, 0.0f, -10.0f) };

//存储元数据的原始值的结构体，用于构造对应的元数据结构体
struct DirectionalLightExample{
   AssignObservable<glm::vec3> color;
   AssignObservable<glm::vec3> direction;
};
// 元数据结构体，存储元数据的引用
struct DirectionalLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& direction;
   DirectionalLightMeta(): color(defaultDirectionalLightColor), direction(defaultDirectionalLightDirection){}
   template<typename ValueStruct>
   DirectionalLightMeta(const ValueStruct& value):color(value.color), direction(value.direction){}
};

inline DirectionalLightMeta defaultDirectionalLightMeta;

struct PointLightExample{
   ObservableValue<glm::vec3>& color;
   ObservableValue<glm::vec3>& position;
   ObservableValue<int> distance;
};
struct PointLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& position;
   const ObservableValue<int> distance;
   template<typename ValueStruct>
   PointLightMeta(const ValueStruct& value):color(value.color), position(value.position), distance(value.distance){}
};
struct SpotLightExample{
   ObservableValue<glm::vec3>& color;
   ObservableValue<int>& distance;
   ObservableValue<glm::vec3>& position;
   ObservableValue<glm::vec3>& direction;
   ObservableValue<float>& outerCutOffDegree;
   ObservableValue<float>& innerCutOffDegree;
};
struct SpotLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& position;
   const ObservableValue<glm::vec3>& direction;
   const ObservableValue<int>& distance;
   const ObservableValue<float>& outerCutOffDegree;
   const ObservableValue<float>& innerCutOffDegree;
   template<typename ValueStruct>
   SpotLightMeta(const ValueStruct& value):color(value.color), position(value.position), direction(value.direction), 
   distance(value.distance), outerCutOffDegree(value.outerCufOffDegree), innerCutOffDegree(value.innerCutOffDegree){}
};



struct DirectionalLight{
   DirectionalLightMeta meta;
   ReactiveMaterial material;
   ReactiveNormalize directionNormalized;
   DirectionalLight(const DirectionalLightMeta& meta): meta(meta), material(meta.color), directionNormalized(meta.direction){}
};

struct PointLight{
   const PointLightMeta& meta;
   ReactiveMaterial material;
   ReactiveAttenuation attenuation;
   PointLight(const PointLightMeta& meta): meta(meta), material(meta.color), attenuation(meta.distance){}
};

struct SpotLight{
   SpotLightMeta meta;
   ReactiveMaterial material;
   ReactiveAttenuation attenuation;
   ReactiveNormalize directionNormalized;
   ReactiveCos innerCutOff;
   ReactiveCos outerCutOff;
   SpotLight(const SpotLightMeta& meta): meta(meta), material(meta.color), attenuation(meta.distance), 
   directionNormalized(meta.direction), innerCutOff(meta.innerCutOffDegree), outerCutOff(meta.outerCutOffDegree){}
};


struct LightObjectMeta{
   VertexArray& vao;
   Texture2D& diffuseTexture;
   Texture2D& specularTexture;
   const ObservableValue<glm::mat4>& model;
   const int number;
   const float& shininess;
   template<typename ValueStruct>
   LightObjectMeta(const ValueStruct& value):vao(value.vao), diffuseTexture(value.diffuseTexture), specularTexture(value.specularTexture), 
   model(value.model), number(value.number), shininess(value.shininess){}
};

struct LightObject{
   LightObjectMeta meta;
   ReactiveValue<glm::mat3, glm::mat4> normalModel;
   LightObject(const LightObjectMeta& meta): meta(meta), normalModel([](const glm::mat4& model){return ModelComputer::computeNormalModel(model);}, meta.model){}
}; 

template<typename T, typename TT>
concept Iterator = requires(T a, T a2, TT b) {
   std::is_convertible_v<std::iter_value_t<T>, TT>;
   // *a = b;
   std::is_same_v<decltype(++a), T>;
   a == a2;
   a != a2;
};

class LightScene{
private:
   Program program;
   std::vector<std::pair<int, DrawUnit>> drawUnits;
   std::vector<LightObject> lightObjects;
   std::vector<PointLight> pointLights;
   std::vector<SpotLight> spotLights;
   DirectionalLight directionalLight;
   const glm::mat4& projection;
   const ObservableValue<glm::mat4>& viewModel;
   ReactiveValue<glm::vec3, glm::mat4> viewPos;
   
public:
   LightScene(const glm::mat4& projection, const ObservableValue<glm::mat4>& viewModel, const DirectionalLightMeta& directionalMeta = defaultDirectionalLightMeta): 
   projection(projection), viewModel(viewModel), directionalLight(directionalMeta), 
   viewPos([](const glm::mat4& viewModel){
      return ModelComputer::computeViewPosition(viewModel);
   }, viewModel),
   program(
      VertexShader::fromFile("../shader/multi_light/cube.vertex.glsl"),
      FragmentShader::fromFile("../shader/multi_light/cube.frag.glsl"))
   {generateDrawUnits();}
   template<typename T>
   requires Iterator<T, PointLightMeta>
   void setPointLights(T lightBegin, T lightEnd){
      pointLights.clear();
      while(lightBegin != lightEnd){
         pointLights.emplace_back(static_cast<PointLightMeta>(*lightBegin));
         ++lightBegin;
      }
      generateDrawUnits();
   }
   template<typename T>
   requires Iterator<T, SpotLightMeta>
   void setSpotLights(T lightBegin, T lightEnd){
      spotLights.clear();
      while(lightBegin != lightEnd){
         spotLights.emplace_back(static_cast<SpotLightMeta>(*lightBegin));
         ++lightBegin;
      }
      generateDrawUnits();
   }
   template<typename T>
   requires Iterator<T, LightObjectMeta>
   void setLightObjects(T objectBegin, T objectEnd){
      lightObjects.clear();
      while(objectBegin != objectEnd){
         lightObjects.emplace_back(static_cast<LightObjectMeta>(*objectBegin));
         ++objectBegin;
      }
      generateDrawUnits();
   }
   void generateDrawUnits(){
      Drawer& drawer = Drawer::getInstance(); 
      for(auto& pair: drawUnits){
         drawer.removeDrawUnit(pair.first);
      }
      for(auto& lightObject: lightObjects){
         DrawUnit drawUnit {lightObject.meta.vao, program, GL_TRIANGLES, lightObject.meta.number};


         drawUnit.addUniform("model", lightObject.meta.model.get());
         drawUnit.addUniform("view", viewModel.get());
         drawUnit.addUniform("projection", projection);


         drawUnit.addUniform("viewPos", viewPos.get());
         drawUnit.addUniform("normalModel", lightObject.normalModel.get());

         drawUnit.addTexture(lightObject.meta.diffuseTexture, 0);
         drawUnit.addTexture(lightObject.meta.specularTexture, 1);
         drawUnit.addUniform("material.diffuse", 0);
         drawUnit.addUniform("material.specular", 1);
         drawUnit.addUniform("material.shininess", lightObject.meta.shininess);
         
         drawUnit.addUniform("directionalLight.direction", directionalLight.directionNormalized.get());
         drawUnit.addUniform("directionalLight.ambient", directionalLight.material->ambient);
         drawUnit.addUniform("directionalLight.diffuse", directionalLight.material->diffuse);
         drawUnit.addUniform("directionalLight.specular", directionalLight.material->specular);

         drawUnit.addUniform("pointLightNum", static_cast<int>(pointLights.size()));
         for(int i = 0; i < pointLights.size(); i++){
            auto& pointLight = pointLights[i];
            drawUnit.addUniform(fmt::format("pointLights[{}].position", i), pointLight.meta.position.get());
            drawUnit.addUniform(fmt::format("pointLights[{}].ambient", i), pointLight.material->ambient);
            drawUnit.addUniform(fmt::format("pointLights[{}].diffuse", i), pointLight.material->diffuse);
            drawUnit.addUniform(fmt::format("pointLights[{}].specular", i), pointLight.material->specular);
            drawUnit.addUniform(fmt::format("pointLights[{}].constant", i), pointLight.attenuation->constant);
            drawUnit.addUniform(fmt::format("pointLights[{}].linear", i), pointLight.attenuation->linear);
            drawUnit.addUniform(fmt::format("pointLights[{}].quadratic", i), pointLight.attenuation->quadratic);
         }
         drawUnit.addUniform("spotLightNum", static_cast<int>(spotLights.size()));
         for(int i = 0; i < spotLights.size(); i++){
            auto& spotLight = spotLights[i];
            drawUnit.addUniform(fmt::format("spotLights[{}].position", i), spotLight.meta.position.get());
            drawUnit.addUniform(fmt::format("spotLights[{}].direction", i), spotLight.directionNormalized.get());
            drawUnit.addUniform(fmt::format("spotLights[{}].outerCutOff", i), spotLight.outerCutOff.get());
            drawUnit.addUniform(fmt::format("spotLights[{}].innerCutOff", i), spotLight.innerCutOff.get());
            drawUnit.addUniform(fmt::format("spotLights[{}].ambient", i), spotLight.material->ambient);
            drawUnit.addUniform(fmt::format("spotLights[{}].diffuse", i), spotLight.material->diffuse);
            drawUnit.addUniform(fmt::format("spotLights[{}].specular", i), spotLight.material->specular);
            drawUnit.addUniform(fmt::format("spotLights[{}].constant", i), spotLight.attenuation->constant);
            drawUnit.addUniform(fmt::format("spotLights[{}].linear", i), spotLight.attenuation->linear);
            drawUnit.addUniform(fmt::format("spotLights[{}].quadratic", i), spotLight.attenuation->quadratic);
         }
         drawUnits.push_back(std::pair{0, std::move(drawUnit)});
         int id = drawer.addDrawUnit(drawUnits.back().second);
         drawUnits.back().first = id;
      }
   }
};

} // namespace minecpp


#endif // _MINECPP_LIGHT_H_
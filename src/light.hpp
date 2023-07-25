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
#include "transformation.hpp"

namespace minecpp
{
   
float vertices[] = {
   // positions
   -0.5f, -0.5f, -0.5f, 
    0.5f, -0.5f, -0.5f, 
    0.5f,  0.5f, -0.5f, 
    0.5f,  0.5f, -0.5f, 
   -0.5f,  0.5f, -0.5f, 
   -0.5f, -0.5f, -0.5f, 

   -0.5f, -0.5f,  0.5f, 
    0.5f, -0.5f,  0.5f, 
    0.5f,  0.5f,  0.5f, 
    0.5f,  0.5f,  0.5f, 
   -0.5f,  0.5f,  0.5f, 
   -0.5f, -0.5f,  0.5f, 

   -0.5f,  0.5f,  0.5f, 
   -0.5f,  0.5f, -0.5f, 
   -0.5f, -0.5f, -0.5f, 
   -0.5f, -0.5f, -0.5f, 
   -0.5f, -0.5f,  0.5f, 
   -0.5f,  0.5f,  0.5f, 

    0.5f,  0.5f,  0.5f, 
    0.5f,  0.5f, -0.5f, 
    0.5f, -0.5f, -0.5f, 
    0.5f, -0.5f, -0.5f, 
    0.5f, -0.5f,  0.5f, 
    0.5f,  0.5f,  0.5f, 

   -0.5f, -0.5f, -0.5f, 
    0.5f, -0.5f, -0.5f, 
    0.5f, -0.5f,  0.5f, 
    0.5f, -0.5f,  0.5f, 
   -0.5f, -0.5f,  0.5f, 
   -0.5f, -0.5f, -0.5f, 

   -0.5f,  0.5f, -0.5f, 
    0.5f,  0.5f, -0.5f, 
    0.5f,  0.5f,  0.5f, 
    0.5f,  0.5f,  0.5f, 
   -0.5f,  0.5f,  0.5f, 
   -0.5f,  0.5f, -0.5f,
};

class LightContext: public ProactiveSingleton<LightContext>{
public:
   Program objectProgram;
   Program lightProgram;
   VertexBuffer lightVbo;
   VertexArray lightVao;
   int lightVertexNumber;
   glm::mat4 scale;
   LightContext(): objectProgram{
      VertexShader::fromFile("../shader/multi_light/cube.vertex.glsl"),
      FragmentShader::fromFile("../shader/multi_light/cube.frag.glsl")
   }, lightProgram{
      VertexShader::fromFile("../shader/multi_light/light.vertex.glsl"),
      FragmentShader::fromFile("../shader/multi_light/light.frag.glsl")
   },
   lightVbo{vertices, sizeof(vertices)},
   lightVertexNumber(64),
   scale{glm::scale(glm::vec3{0.3f})}
   {
      lightVao.addAttribute(lightVbo, 0, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), (const void*)0);
   }
};

struct Attenuation {
   float constant;
   float linear;
   float quadratic;
};

Attenuation computeAttenuation(float distance){
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

struct ReactiveAttenuation: public ReactiveValue<Attenuation, float>{
   ReactiveAttenuation(const ObservableValue<float>& distance): ReactiveValue<Attenuation, float>(computeAttenuation, distance){};
};


struct LightMaterial{
   glm::vec3 diffuse;
   glm::vec3 ambient;
   glm::vec3 specular;
};

LightMaterial computeLightMaterial(const glm::vec3& color){
   auto lightDiffuse = color * glm::vec3(0.5f);
   auto lightAmbient = color * glm::vec3(0.1f);
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

struct ReactiveCutOff: public ReactiveValue<float, float>{
   ReactiveCutOff(const ObservableValue<float>& cutOffDegree): ReactiveValue<float, float>([](float cutOffDegree){return glm::cos(glm::radians(cutOffDegree));}, cutOffDegree){};
};

struct ReactiveLightModel: public ReactiveValue<glm::mat4, glm::vec3>{
   ReactiveLightModel(const ObservableValue<glm::vec3>& position): ReactiveValue<glm::mat4, glm::vec3>([](const glm::vec3& position){
      return newModel(position) * LightContext::getInstance().scale;
   }, position){};
};

inline ChangeableObservable<glm::vec3> defaultDirectionalLightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
inline ChangeableObservable<glm::vec3> defaultDirectionalLightDirection { glm::vec3(0.0f, 0.0f, -10.0f) };


// 元数据结构体，存储元数据的引用
struct DirectionalLightMeta{
   const ObservableValue<glm::vec3>& color {defaultDirectionalLightColor};
   const ObservableValue<glm::vec3>& direction {defaultDirectionalLightDirection};
   // DirectionalLightMeta(): color(defaultDirectionalLightColor), direction(defaultDirectionalLightDirection){}
   // template<typename ValueStruct>
   // DirectionalLightMeta(const ValueStruct& value):color(value.color), direction(value.direction){}
};



inline DirectionalLightMeta defaultDirectionalLightMeta;


struct PointLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& position;
   const ObservableValue<float>& distance;
   // template<typename ValueStruct>
   // PointLightMeta(const ValueStruct& value):color(value.color), position(value.position), distance(value.distance){}
};

struct SpotLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& position;
   const ObservableValue<glm::vec3>& direction;
   const ObservableValue<float>& distance;
   const ObservableValue<float>& outerCutOffDegree;
   const ObservableValue<float>& innerCutOffDegree;
   // template<typename ValueStruct>
   // SpotLightMeta(const ValueStruct& value):color(value.color), position(value.position), direction(value.direction), 
   // distance(value.distance), outerCutOffDegree(value.outerCutOffDegree), innerCutOffDegree(value.innerCutOffDegree){}
};


struct LightObjectMeta{
   VertexArray& vao;
   Texture2D& diffuseTexture;
   Texture2D& specularTexture;
   const ObservableValue<glm::mat4>& model;
   const int number;
   const float& shininess;
   // template<typename ValueStruct>
   // LightObjectMeta(const ValueStruct& value):vao(value.vao), diffuseTexture(value.diffuseTexture), specularTexture(value.specularTexture), 
   // model(value.model), number(value.number), shininess(value.shininess){}
};

class LightObject;
class SpotLight;
class PointLight;
class DirectionalLight;

template<typename T, typename TT>
concept Iterator = requires(T a, T a2, TT b) {
   std::is_convertible_v<std::iter_value_t<T>, TT>;
   // *a = b;
   std::is_same_v<decltype(++a), T>;
   a == a2;
   a != a2;
};

struct BasicData{
   ChangeableObservable<glm::mat4> viewModel {newViewModel(glm::vec3(3.0f, 0.0f, 3.0f))};
   ProjectionCoord projectionCoord;
   ModelMoveSetter modelSetter {viewModel};
};



class LightScene{
friend class DirectionalLight;
friend class SpotLight;
friend class PointLight;
friend class LightObject;
private:
   std::vector<DrawUnit> drawUnits;
   IdContainer<DirectionalLight&> directionalLights;
   // DirectionalLight directionalLight;
   IdContainer<LightObject&> lightObjects;
   IdContainer<PointLight&> pointLights;
   IdContainer<SpotLight&> spotLights;
   const glm::mat4& projection;
   const ObservableValue<glm::mat4>& viewModel;
   ReactiveValue<glm::vec3, glm::mat4> viewPos;
   
public:
   LightScene(const glm::mat4& projection, const ObservableValue<glm::mat4>& viewModel): 
   projection(projection), viewModel(viewModel), /* directionalLight(directionalMeta), */
   viewPos([](const glm::mat4& viewModel){
      return ModelComputer::computeViewPosition(viewModel);
   }, viewModel)
   {
      // generateDrawUnits();
   }
   
   LightScene(BasicData& basicData): LightScene(basicData.projectionCoord.getProjection(), basicData.viewModel){}

   // template<typename T>
   // requires Iterator<T, PointLightMeta>
   // void setPointLights(T lightBegin, T lightEnd){
   //    pointLights.clear();
   //    while(lightBegin != lightEnd){
   //       pointLights.add(static_cast<PointLightMeta>(*lightBegin));
   //       ++lightBegin;
   //    }
   //    generateDrawUnits();
   // }
   // template<typename T>
   // requires Iterator<T, SpotLightMeta>
   // void setSpotLights(T lightBegin, T lightEnd){
   //    spotLights.clear();
   //    while(lightBegin != lightEnd){
   //       spotLights.add(static_cast<SpotLightMeta>(*lightBegin));
   //       ++lightBegin;
   //    }
   //    generateDrawUnits();
   // }
   // template<typename T>
   // requires Iterator<T, LightObjectMeta>
   // void setLightObjects(T objectBegin, T objectEnd){
   //    lightObjects.clear();
   //    while(objectBegin != objectEnd){
   //       lightObjects.add(static_cast<LightObjectMeta>(*objectBegin));
   //       ++objectBegin;
   //    }
   //    generateDrawUnits();
   // }
   void generateDrawUnits();
};

struct DirectionalLight: public AutoLoad<DirectionalLight>{
friend class LightScene;
private:
   DirectionalLightMeta meta;
   ReactiveMaterial material;
   ReactiveNormalize directionNormalized;
public:
   DirectionalLight(const DirectionalLightMeta& meta, LightScene& lightScene): meta(meta), material(meta.color), directionNormalized(meta.direction),
   AutoLoad<DirectionalLight>([&lightScene]() -> auto& {
      return lightScene.directionalLights;
   }){}
};

class SpotLight: public AutoLoad<SpotLight>{
friend class LightScene;
private:
   SpotLightMeta meta;
   ReactiveMaterial material;
   ReactiveAttenuation attenuation;
   ReactiveNormalize directionNormalized;
   ReactiveCutOff innerCutOff;
   ReactiveCutOff outerCutOff;
   ReactiveLightModel model;
public:
   SpotLight(const SpotLightMeta& meta, LightScene& lightScene): meta(meta), material(meta.color), attenuation(meta.distance), 
   directionNormalized(meta.direction), innerCutOff(meta.innerCutOffDegree), outerCutOff(meta.outerCutOffDegree),
   model(meta.position), 
   AutoLoad<SpotLight>([&lightScene]() -> auto& {
      return lightScene.spotLights;
   }){}
};

class PointLight: public AutoLoad<PointLight>{
friend class LightScene;
private:
   PointLightMeta meta;
   ReactiveMaterial material;
   ReactiveAttenuation attenuation;
   ReactiveLightModel model;
public:
   PointLight(const PointLightMeta& meta, LightScene& lightScene): meta(meta), material(meta.color), attenuation(meta.distance),
   model(meta.position), 
   AutoLoad<PointLight>([&lightScene]() -> auto& {
      return lightScene.pointLights;
   }){}
};

class LightObject: public AutoLoad<LightObject>{
friend class LightScene;
private:
   LightObjectMeta meta;
   ReactiveValue<glm::mat3, glm::mat4> normalModel;
public:
   // LightObject(LightObject&&) noexcept = default;
   LightObject(const LightObjectMeta& meta, LightScene& lightScene): meta(meta), 
   normalModel([](const glm::mat4& model){return ModelComputer::computeNormalModel(model);}, meta.model), 
   AutoLoad<LightObject>([&lightScene]() -> auto& {
      return lightScene.lightObjects;
   }){}
};

//存储元数据的原始值的结构体，用于构造对应的元数据结构体
struct DirectionalLightData{
   ChangeableObservable<glm::vec3> color {glm::vec3(1.0f, 1.0f, 1.0f)};
   ChangeableObservable<glm::vec3> direction = {glm::vec3(0.0f, 3.0f, 0.0f)};

   operator DirectionalLightMeta(){
      return {color, direction, };
   }
};

struct PointLightData{
   ChangeableObservable<glm::vec3> color;
   ChangeableObservable<glm::vec3> position;
   ChangeableObservable<float> distance;
   operator PointLightMeta(){
      return {color, position, distance, };
   }
};

struct SpotLightData{
   ChangeableObservable<glm::vec3> color;
   ChangeableObservable<glm::vec3> position;
   ChangeableObservable<glm::vec3> direction;
   ChangeableObservable<float> distance;
   ChangeableObservable<float> outerCutOffDegree;
   ChangeableObservable<float> innerCutOffDegree;

   operator SpotLightMeta(){
      return {color, position, direction, distance, outerCutOffDegree, innerCutOffDegree, };
   }
};


void LightScene::generateDrawUnits(){
   Drawer& drawer = Drawer::getInstance(); 
   drawUnits.clear();
   for(auto& lightObject: lightObjects){
      DrawUnit drawUnit {lightObject.meta.vao, LightContext::getInstance().objectProgram, lightObject.meta.number};


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
      
      {
         drawUnit.addUniform("directionalLightNum", static_cast<int>(directionalLights.size()));
         int i = 0;
         for(auto& directionalLight: directionalLights){
            drawUnit.addUniform(fmt::format("directionalLights[{}].direction", i), directionalLight.directionNormalized.get());
            drawUnit.addUniform(fmt::format("directionalLights[{}].ambient", i), directionalLight.material->ambient);
            drawUnit.addUniform(fmt::format("directionalLights[{}].diffuse", i), directionalLight.material->diffuse);
            drawUnit.addUniform(fmt::format("directionalLights[{}].specular", i), directionalLight.material->specular);
            i++;
         }
      }

      {
         drawUnit.addUniform("pointLightNum", static_cast<int>(pointLights.size()));
         int i = 0;
         for(auto& pointLight: pointLights){
            drawUnit.addUniform(fmt::format("pointLights[{}].position", i), pointLight.meta.position.get());
            drawUnit.addUniform(fmt::format("pointLights[{}].ambient", i), pointLight.material->ambient);
            drawUnit.addUniform(fmt::format("pointLights[{}].diffuse", i), pointLight.material->diffuse);
            drawUnit.addUniform(fmt::format("pointLights[{}].specular", i), pointLight.material->specular);
            drawUnit.addUniform(fmt::format("pointLights[{}].constant", i), pointLight.attenuation->constant);
            drawUnit.addUniform(fmt::format("pointLights[{}].linear", i), pointLight.attenuation->linear);
            drawUnit.addUniform(fmt::format("pointLights[{}].quadratic", i), pointLight.attenuation->quadratic);
            i++;
         }
      }
      {  
         drawUnit.addUniform("spotLightNum", static_cast<int>(spotLights.size()));
         int i = 0;
         for(auto& spotLight :spotLights){
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
            i++;
         }
      }
      drawUnits.emplace_back(std::move(drawUnit));
   }
   LightContext& context = LightContext::getInstance();
   {
      int i = 0;
      for(auto& pointLight : pointLights){
         DrawUnit drawUnit {context.lightVao, context.lightProgram, context.lightVertexNumber};
         drawUnit.addUniform("model", pointLight.model.get());
         drawUnit.addUniform("view", viewModel.get());
         drawUnit.addUniform("projection", projection);
         drawUnit.addUniform("color", pointLight.meta.color.get());
         drawUnits.emplace_back(std::move(drawUnit));
         i++;
      }
   }
   {
      int i = 0;
      for(auto& spotLight: spotLights){
         DrawUnit drawUnit {context.lightVao, context.lightProgram, context.lightVertexNumber};
         drawUnit.addUniform("model", spotLight.model.get());
         drawUnit.addUniform("view", viewModel.get());
         drawUnit.addUniform("projection", projection);
         drawUnit.addUniform("color", spotLight.meta.color.get());
         drawUnits.emplace_back(std::move(drawUnit));
         i++;
      }
   }
   
}

} // namespace minecpp


#endif // _MINECPP_LIGHT_H_
#ifndef _MINECPP_LIGHT_H_
#define _MINECPP_LIGHT_H_

#include <initializer_list>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "vertex.hpp"
#include "tool.hpp"
#include "resource.hpp"
#include "transformation.hpp"

namespace minecpp
{
   
inline std::array<glm::vec3, 36> vertices = {
   // positions
   glm::vec3{-0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 

   {-0.5f, -0.5f,  0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 

   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 

   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 

   {-0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 

   {-0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f, -0.5f},
};

class LightContext: public ProactiveSingleton<LightContext>{
public:
   Program objectProgram;
   Program lightProgram;
   VertexData<false> lightVertex;
   glm::mat4 scale;
   
   LightContext(): 
      objectProgram{
         VertexShader::fromFile("../shader/multi_light/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/multi_light/cube.frag.glsl")
      }, 
      lightProgram{
         VertexShader::fromFile("../shader/multi_light/light.vertex.glsl"),
         FragmentShader::fromFile("../shader/multi_light/light.frag.glsl")
      },
      lightVertex{createVertexData(VertexMeta<false, glm::vec3>{
         .vertexs {vertices.begin(), vertices.end()}
      })},
      scale{glm::scale(glm::vec3{0.3f})}{}
};

struct Attenuation {
   float constant;
   float linear;
   float quadratic;
};

inline Attenuation computeAttenuation(float distance){
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

inline LightMaterial computeLightMaterial(const glm::vec3& color){
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

// 元数据结构体，存储元数据的引用
struct DirectionalLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& direction;
};

struct PointLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& position;
   const ObservableValue<float>& distance;
};

struct SpotLightMeta{
   const ObservableValue<glm::vec3>& color;
   const ObservableValue<glm::vec3>& position;
   const ObservableValue<glm::vec3>& direction;
   const ObservableValue<float>& distance;
   const ObservableValue<float>& outerCutOffDegree;
   const ObservableValue<float>& innerCutOffDegree;
};


struct LightObjectMeta{
   VertexArray& vao;
   Texture2D& diffuseTexture;
   Texture2D* specularTexture;
   const ObservableValue<glm::mat4>& model;
   const float& shininess;
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
   ObservableValue<glm::mat4> viewModel {newViewModel(glm::vec3(3.0f, 0.0f, 3.0f))};
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
   RefContainer<DirectionalLight> directionalLights;
   // DirectionalLight directionalLight;
   RefContainer<LightObject> lightObjects;
   RefContainer<PointLight> pointLights;
   RefContainer<SpotLight> spotLights;
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
   
   LightScene(BasicData& basicData): LightScene(basicData.projectionCoord.projection, basicData.viewModel){}

   void generateDrawUnits();
   void clear() {
      drawUnits.clear();
   }
};

struct DirectionalLight: public AutoLoader<DirectionalLight>{
friend class LightScene;
private:
   DirectionalLightMeta meta;
   ReactiveMaterial material;
   ReactiveNormalize directionNormalized;
public:
   DirectionalLight(const DirectionalLightMeta& meta, LightScene& lightScene): meta(meta), material(meta.color), directionNormalized(meta.direction),
   AutoLoader<DirectionalLight>(lightScene.directionalLights){}
};

class SpotLight: public AutoLoader<SpotLight>{
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
   AutoLoader<SpotLight>(lightScene.spotLights){}
};

class PointLight: public AutoLoader<PointLight>{
friend class LightScene;
private:
   PointLightMeta meta;
   ReactiveMaterial material;
   ReactiveAttenuation attenuation;
   ReactiveLightModel model;
public:
   PointLight(const PointLightMeta& meta, LightScene& lightScene): meta(meta), material(meta.color), attenuation(meta.distance),
   model(meta.position), 
   AutoLoader<PointLight>(lightScene.pointLights){}
};

class LightObject: public AutoLoader<LightObject>{
friend class LightScene;
private:
   LightObjectMeta meta;
   ReactiveValue<glm::mat3, glm::mat4> normalModel;
public:
   // LightObject(LightObject&&) noexcept = default;
   LightObject(const LightObjectMeta& meta, LightScene& lightScene): meta(meta), 
   normalModel([](const glm::mat4& model){return ModelComputer::computeNormalModel(model);}, this->meta.model), 
   AutoLoader<LightObject>(lightScene.lightObjects){}
};

//存储元数据的原始值的结构体，用于构造对应的元数据结构体
struct DirectionalLightData{
   ObservableValue<glm::vec3> color {glm::vec3(1.0f, 1.0f, 1.0f)};
   ObservableValue<glm::vec3> direction {glm::vec3(0.0f, 3.0f, 0.0f)};

   operator DirectionalLightMeta(){
      return {color, direction, };
   }
};

struct PointLightData{
   ObservableValue<glm::vec3> color;
   ObservableValue<glm::vec3> position;
   ObservableValue<float> distance;
   operator PointLightMeta(){
      return {color, position, distance, };
   }
};

struct SpotLightData{
   ObservableValue<glm::vec3> color;
   ObservableValue<glm::vec3> position;
   ObservableValue<glm::vec3> direction;
   ObservableValue<float> distance;
   ObservableValue<float> outerCutOffDegree;
   ObservableValue<float> innerCutOffDegree;

   operator SpotLightMeta(){
      return {color, position, direction, distance, outerCutOffDegree, innerCutOffDegree, };
   }
};


inline void LightScene::generateDrawUnits(){
   Drawer& drawer = Drawer::getInstance(); 
   drawUnits.clear();
   for(auto& lightObject: lightObjects){
      std::vector<DrawUnit::TextureParam> textures;
      std::vector<DrawUnit::UniformParam> uniforms;
      // DrawUnit drawUnit {lightObject.meta.vao, LightContext::getInstance().objectProgram};

      uniforms.emplace_back("model", lightObject.meta.model.get());
      uniforms.emplace_back("view", viewModel.get());
      uniforms.emplace_back("projection", projection);


      uniforms.emplace_back("viewPos", viewPos.get());
      uniforms.emplace_back("normalModel", lightObject.normalModel.get());

      textures.emplace_back(0, "material.diffuse", lightObject.meta.diffuseTexture);
      if(lightObject.meta.specularTexture != nullptr){
         textures.emplace_back(1, "material.specular", *lightObject.meta.specularTexture);
      }else{
         uniforms.emplace_back("material.specular", 1);
      }
      uniforms.emplace_back("material.shininess", lightObject.meta.shininess);
      
      uniforms.emplace_back("directionalLightNum", static_cast<int>(directionalLights.size()));
      for(int i = 0; auto& directionalLight: directionalLights){
         uniforms.emplace_back(fmt::format("directionalLights[{}].direction", i), directionalLight.directionNormalized.get());
         uniforms.emplace_back(fmt::format("directionalLights[{}].ambient", i), directionalLight.material->ambient);
         uniforms.emplace_back(fmt::format("directionalLights[{}].diffuse", i), directionalLight.material->diffuse);
         uniforms.emplace_back(fmt::format("directionalLights[{}].specular", i), directionalLight.material->specular);
         i++;
      }

      uniforms.emplace_back("pointLightNum", static_cast<int>(pointLights.size()));
      for(int i = 0; auto& pointLight: pointLights){
         uniforms.emplace_back(fmt::format("pointLights[{}].position", i), pointLight.meta.position.get());
         uniforms.emplace_back(fmt::format("pointLights[{}].ambient", i), pointLight.material->ambient);
         uniforms.emplace_back(fmt::format("pointLights[{}].diffuse", i), pointLight.material->diffuse);
         uniforms.emplace_back(fmt::format("pointLights[{}].specular", i), pointLight.material->specular);
         uniforms.emplace_back(fmt::format("pointLights[{}].constant", i), pointLight.attenuation->constant);
         uniforms.emplace_back(fmt::format("pointLights[{}].linear", i), pointLight.attenuation->linear);
         uniforms.emplace_back(fmt::format("pointLights[{}].quadratic", i), pointLight.attenuation->quadratic);
         i++;
      }

      uniforms.emplace_back("spotLightNum", static_cast<int>(spotLights.size()));
      for(int i = 0; auto& spotLight :spotLights){
         uniforms.emplace_back(fmt::format("spotLights[{}].position", i), spotLight.meta.position.get());
         uniforms.emplace_back(fmt::format("spotLights[{}].direction", i), spotLight.directionNormalized.get());
         uniforms.emplace_back(fmt::format("spotLights[{}].outerCutOff", i), spotLight.outerCutOff.get());
         uniforms.emplace_back(fmt::format("spotLights[{}].innerCutOff", i), spotLight.innerCutOff.get());
         uniforms.emplace_back(fmt::format("spotLights[{}].ambient", i), spotLight.material->ambient);
         uniforms.emplace_back(fmt::format("spotLights[{}].diffuse", i), spotLight.material->diffuse);
         uniforms.emplace_back(fmt::format("spotLights[{}].specular", i), spotLight.material->specular);
         uniforms.emplace_back(fmt::format("spotLights[{}].constant", i), spotLight.attenuation->constant);
         uniforms.emplace_back(fmt::format("spotLights[{}].linear", i), spotLight.attenuation->linear);
         uniforms.emplace_back(fmt::format("spotLights[{}].quadratic", i), spotLight.attenuation->quadratic);
         i++;
      }
      
      drawUnits.emplace_back(
         lightObject.meta.vao, 
         LightContext::getInstance().objectProgram, 
         uniforms,
         textures
      );
   }
   auto& context = LightContext::getInstance();
   for(auto& pointLight : pointLights){
      std::vector<DrawUnit::UniformParam> uniforms;
      uniforms.emplace_back("model", pointLight.model.get());
      uniforms.emplace_back("view", viewModel.get());
      uniforms.emplace_back("projection", projection);
      uniforms.emplace_back("color", pointLight.meta.color.get());
      drawUnits.emplace_back(
         context.lightVertex.vao, 
         context.lightProgram, 
         uniforms,
         std::vector<DrawUnit::TextureParam>{}
      );
   }

   for(auto& spotLight: spotLights){
      std::vector<DrawUnit::UniformParam> uniforms;
      uniforms.emplace_back("model", spotLight.model.get());
      uniforms.emplace_back("view", viewModel.get());
      uniforms.emplace_back("projection", projection);
      uniforms.emplace_back("color", spotLight.meta.color.get());
      drawUnits.emplace_back(
         context.lightVertex.vao, 
         context.lightProgram, 
         uniforms,
         std::vector<DrawUnit::TextureParam>{}
      );
   }
   
}

} // namespace minecpp


#endif // _MINECPP_LIGHT_H_
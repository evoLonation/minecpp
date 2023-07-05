#ifndef _MINECPP_LIGHT_CASTER_H_
#define _MINECPP_LIGHT_CASTER_H_

#include "../resource.hpp"
#include "../imgui.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>


namespace light_caster
{
using namespace minecpp;
   
float vertices[] = {
   // positions          // normals           // texture coords
   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

enum class LightType{
   DIRECTIONAL, ATTENUATION, FLASHLIGHT
};

template<typename T>
void showPopup(T& t, std::map<T, std::string> elements){
   if (ImGui::Button("Select..")){
      ImGui::OpenPopup("my_select_popup");
   }
   ImGui::SameLine();
   ImGui::TextUnformatted(elements[t].c_str());
   if (ImGui::BeginPopup("my_select_popup")){
      ImGui::SeparatorText("Aquarium");
      for(auto& element: elements){
         if(ImGui::Selectable(element.second.c_str())){
            t = element.first;
         }
      }
      ImGui::EndPopup();
   }
}

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

glm::mat4 newViewModel(const glm::vec3& position, const glm::vec3& target = glm::vec3(0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)){
   // 从相机的方向上看过去，其右边是x轴正方向，上边是y轴正方向，前边是z轴反方向
   // 使用glm::lookAt方法，给定相机位置、目标（相机对准的地方，z轴反方向）与y轴方向（都是相对于世界坐标系），得到相机的view矩阵
   // 参数无需单位化
   return glm::lookAt(position, target - position, up);
}
glm::mat4 newModel(const glm::vec3& location, const glm::vec3& scale){
   return glm::translate(glm::mat4(1.0f), location) * glm::scale(scale);
}
glm::mat4 newModel(const glm::vec3& location = glm::vec3(), float scale = 1.0f){
   return newModel(location, glm::vec3(scale, scale, scale));
}


class ModelController{
private:
   AssignObservable<glm::mat4>& model;
public:
   // model必须是空间矩阵，否则是未定义行为
   ModelController(AssignObservable<glm::mat4>& model): model(model){}

   void translate(const glm::vec3& vec){
      // 相对于目标坐标系进行平移
      model = glm::translate(vec) * model.get();
   }
   void translateX(float delta){
      translate({delta, 0.0f, 0.0f});
   }
   void translateY(float delta){
      translate({0.0f, delta, 0.0f});
   }
   void translateZ(float delta){
      translate({0.0f, 0.0f, delta});
   }
   void rotate(float angle, const glm::vec3& axios){
      model = glm::mat4_cast(glm::angleAxis(glm::radians(angle), axios)) * model.get();
      // model = glm::mat4_cast(glm::angleAxis(glm::radians(angle), axios)) * model;
   }
   void rotateX(float angle){
      rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
   }
   void rotateY(float angle){
      rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
   }
   void rotateZ(float angle){
      rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
   }
};

class ModelComputer{
public:
   static glm::vec3 computePosition(const glm::mat4& model){
      return glm::vec3(model[3]);
   }
   static glm::vec3 computeViewPosition(const glm::mat4& viewModel){
      return glm::vec3(glm::inverse(viewModel)[3]);
   }
   static glm::mat3 computeNormalModel(const glm::mat4& model){
      return glm::mat3(glm::transpose(glm::inverse(model)));
   }
};

// 该类的含有的资源包括“对其他资源的设置”，因此不能轻易拷贝和移动
class ProjectionCoord: public UnCopyMoveable{
private:
   glm::mat4 projection;
public:
   ProjectionCoord(float fovy = 45.0f, float near = 0.1f, float far = 100.0f){
      Context& ctx = Context::getInstance(); 
      auto& width = ctx.getWidth();
      auto& height = ctx.getHeight();
      auto computePj = [fovy, near, far, this](){
         Context& ctx = Context::getInstance(); 
         auto width = ctx.getWidth().get();
         auto height = ctx.getHeight().get();
         this->projection = glm::perspective(glm::radians(fovy), width * 1.0f / height , near, far);
      };
      computePj();
      width.addObserver(computePj);
      height.addObserver(computePj);
   }

   const glm::mat4& getProjection() const {
      return projection;
   }
};

class CameraMoveSetter: public UnCopyMoveable{
private:
   std::vector<KeyHoldHandlerSetter> handlerSetters;
   ModelController viewModelController;
public:
   float moveSpeed = 0.05f;
   float rotateSpeed = 1.0f;
   CameraMoveSetter(AssignObservable<glm::mat4>& viewModel):viewModelController(viewModel){
      auto add = [this](int key, const std::function<void()>& handler){
         this->handlerSetters.emplace_back(key, [handler](auto _){handler();});
      };
      add(GLFW_KEY_A, [this](){this->viewModelController.translateX(moveSpeed);});
      add(GLFW_KEY_D, [this](){this->viewModelController.translateX(-moveSpeed);});
      add(GLFW_KEY_Z, [this](){this->viewModelController.translateY(-moveSpeed);});
      add(GLFW_KEY_X, [this](){this->viewModelController.translateY(moveSpeed);});
      add(GLFW_KEY_W, [this](){this->viewModelController.translateZ(moveSpeed);});
      add(GLFW_KEY_S, [this](){this->viewModelController.translateZ(-moveSpeed);});
      add(GLFW_KEY_L, [this](){this->viewModelController.rotateY(rotateSpeed);});
      add(GLFW_KEY_J, [this](){this->viewModelController.rotateY(-rotateSpeed);});
      add(GLFW_KEY_I, [this](){this->viewModelController.rotateX(-rotateSpeed);});
      add(GLFW_KEY_K, [this](){this->viewModelController.rotateX(rotateSpeed);});
      add(GLFW_KEY_U, [this](){this->viewModelController.rotateZ(-rotateSpeed);});
      add(GLFW_KEY_O, [this](){this->viewModelController.rotateZ(rotateSpeed);});
   }
};

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

int run(){
   try{
      RefDirtyObservable type = LightType::DIRECTIONAL;
      
      Context ctx {800, 600};
      InputProcessor processor;
      Drawer drawer;

      /*********     opengl resource part      *********/ 
      Program cubeDirectionalProgram {
         VertexShader::fromFile("../shader/light_caster/directional/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/light_caster/directional/cube.frag.glsl")
      };
      Program cubeAttenuationProgram {
         VertexShader::fromFile("../shader/light_caster/attenuation/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/light_caster/attenuation/cube.frag.glsl")
      };
      Program cubeFlashLightProgram {
         VertexShader::fromFile("../shader/light_caster/flashlight/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/light_caster/flashlight/cube.frag.glsl")
      };
      // for attenuation
      Program lightProgram {
         VertexShader::fromFile("../shader/light_caster/light.vertex.glsl"),
         FragmentShader::fromFile("../shader/light_caster/light.frag.glsl")
      };

      VertexBuffer vbo {vertices, sizeof(vertices)};
      VertexArray vao;
      vao.addAttribute(vbo, 0, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)0);
      vao.addAttribute(vbo, 1, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));
      vao.addAttribute(vbo, 2, 2, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat)));

      Texture2D texture {"../image/container2.png"};
      Texture2D specular {"../image/container2_specular.png"};

      /*********     uniform data part      *********/ 
      glm::vec3 cubePositions[] = {
         glm::vec3( 0.0f,  0.0f,  0.0f),
         glm::vec3( 2.0f,  5.0f, -15.0f),
         glm::vec3(-1.5f, -2.2f, -2.5f),
         glm::vec3(-3.8f, -2.0f, -12.3f),
         glm::vec3( 2.4f, -0.4f, -3.5f),
         glm::vec3(-1.7f,  3.0f, -7.5f),
         glm::vec3( 1.3f, -2.0f, -2.5f),
         glm::vec3( 1.5f,  2.0f, -2.5f),
         glm::vec3( 1.5f,  0.2f, -1.5f),
         glm::vec3(-1.3f,  1.0f, -1.5f)
      };
      // cube model
      std::vector<AssignObservable<glm::mat4>> cubeModels;
      for(int i = 0; i < 10; i++){
         cubeModels.emplace_back(newModel(cubePositions[i]));
      }
      // normal model
      std::vector<ReactiveValue<glm::mat3, glm::mat4>> normalModels;
      for(int i = 0; i < 10; i++){
         normalModels.emplace_back([](const glm::mat4& model){
            return ModelComputer::computeNormalModel(model);
         }, cubeModels[i]);
      }

      // view, view pos
      AssignObservable viewModel {newViewModel(glm::vec3(3.0f, 0.0f, 3.0f))};
      ReactiveValueAuto viewPos {[](const glm::mat4& viewModel){
         return ModelComputer::computeViewPosition(viewModel);
      }, viewModel};
      CameraMoveSetter cameraSetter { viewModel };
      // projection
      ProjectionCoord projectionCoord;

      // light info
      // directional
      ManualObservable lightDirection { glm::vec3(-3.0f, 0.0f, 3.0f) };
      ReactiveValueAuto lightDirNormalized {[](glm::vec3 lightDir){
         return glm::normalize(lightDir);
      }, lightDirection};
      // attenuation
      ManualObservable lightPos {glm::vec3(3.0f, 0.0f, -3.0f)};
      ManualObservable lightScale {1.0f};
      ReactiveValueAuto lightModel {[](const glm::vec3& lightPos, float lightScale){
         return newModel(lightPos, lightScale);
         // lightModel = glm::translate(glm::mat4(1.0f), lightPos.value());
         // lightModel = lightModel * glm::scale(glm::vec3(lightScale.value(), lightScale.value(), lightScale.value()));
      }, lightPos, lightScale};
      
      ManualObservable maxDistance {100};
      ReactiveValue<Attenuation, int> attenuation {[](int maxDistance){
         Attenuation attenuation;
         getAttenuation(maxDistance, attenuation.constant, attenuation.linear, attenuation.quadratic);
         return attenuation;
      }, maxDistance};
      // flash light
      auto computeCutOff = [](float& cutOff, float cutOffDegree){cutOff = glm::cos(glm::radians(cutOffDegree));};
      ManualObservable outerCutOffDegree = 30.0f;
      ReactiveValue<float, float> outerCutOff {[](float cutOffDegree){
         return glm::cos(glm::radians(cutOffDegree));
      }, outerCutOffDegree};
      ManualObservable innerCutOffDegree = 20.0f; 
      ReactiveValue<float, float> innerCutOff {[](float cutOffDegree){
         return glm::cos(glm::radians(cutOffDegree));
      }, innerCutOffDegree};
      
      // common
      ManualObservable lightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
      ReactiveValue<LightMaterial, glm::vec3> lightMaterial {[](const glm::vec3& lightColor){
         auto lightDiffuse = lightColor * glm::vec3(0.5f);
         auto lightAmbient = lightColor * glm::vec3(0.2f);
         auto lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
         return LightMaterial {
            lightDiffuse,
            lightAmbient,
            lightSpecular
         };
      }, lightColor};
      
      /*********     draw unit initialize part      *********/ 
      std::vector<DrawUnit> directionalCubes;
      std::vector<DrawUnit> attenuationCubes;
      std::vector<DrawUnit> flashLightCubes;
      // 预先分配空间，否则引用失效
      directionalCubes.reserve(10);
      attenuationCubes.reserve(10);
      flashLightCubes.reserve(10);
      for(int i = 0; i < 10; i++){
         DrawUnit attenuationCube {vao, cubeAttenuationProgram, GL_TRIANGLES, 64};
         DrawUnit directionalCube {vao, cubeDirectionalProgram, GL_TRIANGLES, 64};
         DrawUnit flashLightCube  {vao, cubeFlashLightProgram,  GL_TRIANGLES, 64};
         auto commonCubeSetter = [&](DrawUnit& cube){
            // cube: material
            float shininess = 64.0f;
            cube.addTexture(texture);
            cube.addTexture(specular, 1);
            cube.addUniform("material.diffuse", 0);
            cube.addUniform("material.specular", 1);
            cube.addUniform("material.shininess", std::move(shininess));
            // cube: light
            cube.addUniform("light.ambient", lightMaterial->ambient);
            cube.addUniform("light.diffuse", lightMaterial->diffuse);
            cube.addUniform("light.specular", lightMaterial->specular);
            // cube: normal model, view pos, model, view, projection
            cube.addUniform("normalModel", normalModels[i].get());
            cube.addUniform("model", cubeModels[i].get());
            cube.addUniform("viewPos", viewPos.get());
            cube.addUniform("view", viewModel.get());
            cube.addUniform("projection", projectionCoord.getProjection());
         };
         commonCubeSetter(directionalCube);
         commonCubeSetter(attenuationCube);
         commonCubeSetter(flashLightCube);
         
         // directional 
         directionalCube.addUniform("light.direction", lightDirNormalized.get());

         // attenuation and flash light
         auto attenuationCubeSetter = [&](DrawUnit& cube){
            cube.addUniform("light.constant", attenuation->constant);
            cube.addUniform("light.linear", attenuation->linear);
            cube.addUniform("light.quadratic", attenuation->quadratic);
            cube.addUniform("light.position", lightPos.get());
         };
         attenuationCubeSetter(attenuationCube);
         attenuationCubeSetter(flashLightCube);
         // flash light
         flashLightCube.addUniform("light.direction", lightDirNormalized.get());
         flashLightCube.addUniform("light.outerCutOff", outerCutOff.get());
         flashLightCube.addUniform("light.innerCutOff", innerCutOff.get());

         // 不要在对vector进行扩充的时候获取vector中元素的引用！！！如果重新分配内存就会导致野指针
         // 除非保证不会重新分配内存
         directionalCubes.push_back(std::move(directionalCube));
         attenuationCubes.push_back(std::move(attenuationCube));
         flashLightCubes.push_back(std::move(flashLightCube));
      }
      
      DrawUnit light{vao, lightProgram, GL_TRIANGLES, 64};
      light.addUniform("model", lightModel.get());
      light.addUniform("view", viewModel.get());
      light.addUniform("projection", projectionCoord.getProjection());
      light.addUniform("color", lightColor.get());

      auto drawerSetter = [&attenuationCubes, &drawer, &light, &directionalCubes, &flashLightCubes](LightType type){
         auto& units = drawer.drawUnits;
         units.clear();
         if(type == LightType::DIRECTIONAL){
            for(auto& cube: directionalCubes){
               units.push_back(cube);
            }
         }else if(type == LightType::ATTENUATION){
            for(auto& cube: attenuationCubes){
               units.push_back(cube);
            }
            units.push_back(light);
         }else if(type == LightType::FLASHLIGHT){
            for(auto& cube: flashLightCubes){
               units.push_back(cube);
            }
            units.push_back(light);
         }
      };
      auto& typeVal = type.value();
      drawerSetter(typeVal);
      type.addObserver([&]{
         drawerSetter(typeVal);
      });


      /*********     gui setting part      *********/ 
      GuiContext guiCtx;
      auto guiDrawer = [&]{
         ImGui::Begin("controller");
         std::map<LightType, std::string> elements;
         elements[LightType::ATTENUATION] = "ATTENUATION";
         elements[LightType::DIRECTIONAL] = "DIRECTIONAL";
         elements[LightType::FLASHLIGHT] = "FLASHLIGHT";
         showPopup(typeVal, elements);
         type.check();
         ImGui::ColorEdit3("light color", glm::value_ptr(lightColor.val()));
         lightColor.mayNotice();
         auto showPointLight = [&]{
            ImGui::SliderFloat("light position: x", &lightPos->x, -20.0f, 20.0f);
            ImGui::SliderFloat("light position: y", &lightPos->y, -20.0f, 20.0f);
            ImGui::SliderFloat("light position: z", &lightPos->z, -20.0f, 20.0f);
            lightPos.mayNotice();
            ImGui::SliderInt("light max distance: ", &maxDistance, 0, 300);
            maxDistance.mayNotice();
            ImGui::SliderFloat("light cube scale: ", &lightScale, 0.0f, 2.0f);
            lightScale.mayNotice();
         };
         auto showDirection = [&]{
            ImGui::SliderFloat("light direction: x", &lightDirection->x, -5.0f, 5.0f);
            ImGui::SliderFloat("light direction: y", &lightDirection->y, -5.0f, 5.0f);
            ImGui::SliderFloat("light direction: z", &lightDirection->z, -5.0f, 5.0f);
            lightDirection.mayNotice();
         };
         if(typeVal == LightType::ATTENUATION){
            showPointLight();
         }else if(typeVal == LightType::DIRECTIONAL){
            showDirection();
         }else if(typeVal == LightType::FLASHLIGHT){
            showPointLight();
            showDirection();
            ImGui::SliderFloat("outer cutoff: ", &outerCutOffDegree, -0.0f, 90.0f);
            ImGui::SliderFloat("inner cutoff: ", &innerCutOffDegree, -0.0f, 90.0f);
            outerCutOffDegree.mayNotice();
            innerCutOffDegree.mayNotice();
         }
         ImGui::End();
      };

      ctx.startLoop([&]{
         GuiFrame guiFrame;
         guiDrawer();
         drawer.draw([&]{guiFrame.render();});
         processor.processInput();
      });

   }catch(std::string e){
      fmt::println("{}", e);
   }
   return 0;
}

} // namespace light_caster


#endif // _MINECPP_LIGHT_CASTER_H_
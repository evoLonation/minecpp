#ifndef _MINECPP_MULTI_LIGHT_H_
#define _MINECPP_MULTI_LIGHT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../resource.hpp"
#include "../gui.hpp"
#include "../light.hpp"
#include "../model.hpp"


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

int run(){
   try{
      Context ctx {1920, 1080};
      InputProcessor processor;
      Drawer drawer;

      /*********     opengl resource part      *********/ 
      Program cubeProgram {
         VertexShader::fromFile("../shader/multi_light/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/multi_light/cube.frag.glsl")
      };
      Program lightProgram {
         VertexShader::fromFile("../shader/multi_light/light.vertex.glsl"),
         FragmentShader::fromFile("../shader/multi_light/light.frag.glsl")
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
      ModelMoveSetter cameraSetter { viewModel };
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

#endif // _MINECPP_MULTI_LIGHT_H_
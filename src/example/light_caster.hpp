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
   DIRECTIONAL, ATTENUATION,
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

int run(){
   try{
      RefDirtyObservable type = LightType::DIRECTIONAL;
      auto& typeVal = type.value();
      Context ctx {800, 600};
      auto& width = ctx.getWidth();
      auto& height = ctx.getHeight();
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
      std::vector<glm::mat4> cubeModels;
      // normal model
      std::vector<glm::mat3> normalModels;
      for(int i = 0; i < 10; i++){
         auto cubeModel = glm::translate(glm::mat4(1.0f), cubePositions[i]);
         cubeModels.push_back(cubeModel);
         normalModels.push_back(glm::mat3(glm::transpose(glm::inverse(cubeModel))));
      }

      // view, view pos
      DirtyObservable viewPos = glm::vec3(3.0f, 0.0f, 3.0f);
      DirtyObservable view = glm::lookAt(viewPos.value(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      view.addObserver([&](){
         viewPos = glm::vec3(glm::inverse(view.value())[3]);
         // viewPos = glm::vec3(glm::inverse(view.value()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
      });
      // projection
      DirtyObservable projection = glm::perspective(glm::radians(45.0f), width.value() * 1.0f / height.value() , 0.1f, 100.0f);
      auto computePj = [&](){
         projection = glm::perspective(glm::radians(45.0f), width.value() * 1.0f / height.value() , 0.1f, 100.0f);
      };
      width.addObserver(computePj);
      height.addObserver(computePj);

      // light info
      // directional
      RefDirtyObservable lightDirection { glm::vec3(-3.0f, 0.0f, 3.0f) };
      auto lightDirNormalized = glm::normalize(lightDirection.value());
      lightDirection.addObserver([&]{
         lightDirNormalized = glm::normalize(lightDirection.value());
      });
      // attenuation
      RefDirtyObservable lightPos {glm::vec3(3.0f, 0.0f, -3.0f)};
      RefDirtyObservable lightScale {1.0f};
      glm::mat4 lightModel;
      auto lightModelAssign = [&]{
         lightModel = glm::translate(glm::mat4(1.0f), lightPos.value());
         lightModel = lightModel * glm::scale(glm::vec3(lightScale.value(), lightScale.value(), lightScale.value()));
      };
      lightModelAssign();
      lightPos.addObserver(lightModelAssign);
      lightScale.addObserver(lightModelAssign);
      
      RefDirtyObservable constant {0.0f};
      RefDirtyObservable linear {0.0f};
      RefDirtyObservable quadratic {0.0f};
      RefDirtyObservable<unsigned> maxDistance {100};
      getAttenuation(maxDistance.value(), constant.value(), linear.value(), quadratic.value());
      maxDistance.addObserver([&]{
         getAttenuation(maxDistance.value(), constant.value(), linear.value(), quadratic.value());
         constant.check();
         linear.check();
         quadratic.check();
      });
      // common
      RefDirtyObservable lightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
      auto lightdiffuse = lightColor.value() * glm::vec3(0.5f);
      auto lightambient = lightdiffuse * glm::vec3(0.2f);
      lightColor.addObserver([&](){
         lightdiffuse = lightColor.value() * glm::vec3(0.5f);
         lightambient = lightdiffuse * glm::vec3(0.2f);
      });
      auto lightspecular = glm::vec3(1.0f, 1.0f, 1.0f);
      
      
      /*********     draw unit initialize part      *********/ 
      std::vector<DrawUnit> directionalCubes;
      std::vector<DrawUnit> attenuationCubes;
      // 预先分配空间，否则引用失效
      directionalCubes.reserve(10);
      attenuationCubes.reserve(10);
      for(int i = 0; i < 10; i++){
         DrawUnit attenuationCube {vao, cubeAttenuationProgram, GL_TRIANGLES, 64};
         DrawUnit directionalCube{vao, cubeDirectionalProgram, GL_TRIANGLES, 64};
         auto commonCubeSetter = [&](DrawUnit& cube){
            // cube: material
            float shininess = 64.0f;
            cube.addTexture(texture);
            cube.addTexture(specular, 1);
            cube.addUniform("material.diffuse", 0);
            cube.addUniform("material.specular", 1);
            cube.addUniform("material.shininess", std::move(shininess));
            // cube: light
            cube.addUniform("light.ambient", lightambient);
            cube.addUniform("light.diffuse", lightdiffuse);
            cube.addUniform("light.specular", lightspecular);
            // cube: normal model, view pos, model, view, projection
            cube.addUniform("normalModel", normalModels[i]);
            cube.addUniform("model", cubeModels[i]);
            cube.addUniform("viewPos", viewPos.value());
            cube.addUniform("view", view.value());
            cube.addUniform("projection", projection.value());
         };
         commonCubeSetter(directionalCube);
         commonCubeSetter(attenuationCube);
         
         // directional 
         directionalCube.addUniform("light.direction", lightDirNormalized);

         // attenuation
         attenuationCube.addUniform("light.constant", constant.value());
         attenuationCube.addUniform("light.linear", linear.value());
         attenuationCube.addUniform("light.quadratic", quadratic.value());
         attenuationCube.addUniform("light.position", lightPos.value());

         // 不要在对vector进行扩充的时候获取vector中元素的引用！！！如果重新分配内存就会导致野指针
         // 除非保证不会重新分配内存
         directionalCubes.push_back(std::move(directionalCube));
         attenuationCubes.push_back(std::move(attenuationCube));
      }
      
      DrawUnit light{vao, lightProgram, GL_TRIANGLES, 64};
      light.addUniform("model", lightModel);
      light.addUniform("view", view.value());
      light.addUniform("projection", projection.value());
      light.addUniform("color", lightColor.value());

      auto drawerSetter = [&]{
         auto& units = drawer.drawUnits;
         units.clear();
         if(typeVal == LightType::DIRECTIONAL){
            for(auto& cube: directionalCubes){
               units.push_back(cube);
            }
         }else if(typeVal == LightType::ATTENUATION){
            for(auto& cube: attenuationCubes){
               units.push_back(cube);
            }
            units.push_back(light);
         }
      };
      drawerSetter();
      type.addObserver(drawerSetter);


      /*********     gui setting part      *********/ 
      GuiContext guiCtx;
      auto guiDrawer = [&]{
         ImGui::Begin("controller");
         std::map<LightType, std::string> elements;
         elements[LightType::ATTENUATION] = "ATTENUATION";
         elements[LightType::DIRECTIONAL] = "DIRECTIONAL";
         showPopup(typeVal, elements);
         type.check();
         auto& lightColorVal = lightColor.value();
         ImGui::ColorEdit3("light color", glm::value_ptr(lightColorVal));
         lightColor.check();
         if(typeVal == LightType::ATTENUATION){
            auto& lightPosVal = lightPos.value();
            ImGui::SliderFloat("light position: x", &lightPosVal.x, -20.0f, 20.0f);
            ImGui::SliderFloat("light position: y", &lightPosVal.y, -20.0f, 20.0f);
            ImGui::SliderFloat("light position: z", &lightPosVal.z, -20.0f, 20.0f);
            lightPos.check();
            ImGui::SliderInt("light max distance: ", &reinterpret_cast<int&>(maxDistance.value()), 0, 300);
            maxDistance.check();
            ImGui::SliderFloat("light cube scale: ", &lightScale.value(), 0.0f, 2.0f);
            lightScale.check();
         }else if(typeVal == LightType::DIRECTIONAL){
            auto& lightDirVal = lightDirection.value();
            ImGui::SliderFloat("light direction: x", &lightDirVal.x, -5.0f, 5.0f);
            ImGui::SliderFloat("light direction: y", &lightDirVal.y, -5.0f, 5.0f);
            ImGui::SliderFloat("light direction: z", &lightDirVal.z, -5.0f, 5.0f);
            lightDirection.check();
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
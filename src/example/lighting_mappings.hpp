#ifndef _MINECPP_LIGHTING_MAPPINGS_H_
#define _MINECPP_LIGHTING_MAPPINGS_H_
#include <gl.h>
#include "../resource.hpp"
#include "../imgui.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace lighting_mappings
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
      Context ctx {800, 600};
      auto& width = ctx.getWidth();
      auto& height = ctx.getHeight();
      InputProcessor processor;
      Drawer drawer;
      Program cubeProgram {
         VertexShader::fromFile("../shader/lightingmapping/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/lightingmapping/cube.frag.glsl")
      };
      Program lightProgram {
         VertexShader::fromFile("../shader/lightingmapping/light.vertex.glsl"),
         FragmentShader::fromFile("../shader/lightingmapping/light.frag.glsl")
      };
      VertexBuffer vbo {vertices, sizeof(vertices)};
      VertexArray vao;
      vao.addAttribute(vbo, 0, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)0);
      vao.addAttribute(vbo, 1, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));
      vao.addAttribute(vbo, 2, 2, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat)));

      Texture2D texture {"../image/container2.png"};
      Texture2D specular {"../image/container2_specular.png"};


      // cube model
      DirtyObservable cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
      cubeModel = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * cubeModel.value();
      // light model, light pos
      RefDirtyObservable lightPos = glm::vec3(3.0f, 0.0f, -3.0f);
      DirtyObservable lightModel = glm::translate(glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), lightPos.value());
      lightPos.addObserver([&](){
         lightModel = glm::translate(glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), lightPos.value());
      });
      // view, view pos
      DirtyObservable viewPos = glm::vec3(1.0f, 0.0f, 1.0f);
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

      // normal model
      DirtyObservable normalModel = glm::mat3(glm::transpose(glm::inverse(cubeModel.value())));
      cubeModel.addObserver([&](){
         normalModel = glm::mat3(glm::transpose(glm::inverse(cubeModel.value())));
      });
      DirtyObservable lightColor = glm::vec3(1.0f, 1.0f, 1.0f);


      DrawUnit cube  {vao, cubeProgram, GL_TRIANGLES, 64};
      DrawUnit light {vao, lightProgram, GL_TRIANGLES, 64};

      // cube: material
      auto shininess = 64.0f;
      cube.addTexture(texture);
      cube.addTexture(specular, 1);
      cube.addUniform("material.diffuse", 0);
      cube.addUniform("material.specular", 1);
      cube.addUniform("material.shininess", shininess);
      // cube: light
      auto lightdiffuse = lightColor.value() * glm::vec3(0.5f);
      auto lightambient = lightdiffuse * glm::vec3(0.2f);
      lightColor.addObserver([&](){
         lightdiffuse = lightColor.value() * glm::vec3(0.5f);
         lightambient = lightdiffuse * glm::vec3(0.2f);
      });
      auto lightspecular = glm::vec3(1.0f, 1.0f, 1.0f);
      cube.addUniform("light.ambient", lightambient);
      cube.addUniform("light.diffuse", lightdiffuse);
      cube.addUniform("light.specular", lightspecular);
      cube.addUniform("light.position", lightPos.value());

      // cube: normal model, color, light color, light pos, view pos
      cube.addUniform("normalModel", normalModel.value());
      cube.addUniform("viewPos", viewPos.value());

      // model, view, projection
      cube.addUniform("model", cubeModel.value());
      light.addUniform("model", lightModel.value());
      cube.addUniform("view", view.value());
      light.addUniform("view", view.value());
      cube.addUniform("projection", projection.value());
      light.addUniform("projection", projection.value());

      light.addUniform("color", lightColor.value());

      drawer.addDrawUnit(cube);
      drawer.addDrawUnit(light);
      GuiContext guiCtx;
      ctx.startLoop([&]{
         GuiFrame guiFrame;
         auto& lightPosVal = lightPos.value();
         ImGui::Begin("controller");
         ImGui::SliderFloat("light position: x", &lightPosVal.x, -5.0f, 5.0f);
         ImGui::SliderFloat("light position: y", &lightPosVal.y, -5.0f, 5.0f);
         ImGui::SliderFloat("light position: z", &lightPosVal.z, -5.0f, 5.0f);
         ImGui::End();
         lightPos.check();
         drawer.draw([&]{guiFrame.render();});
         processor.processInput();
      });

   }catch(std::string e){
      fmt::println("{}", e);
   }
   return 0;
}

} // namespace lighting_mappings


#endif // _MINECPP_LIGHTING_MAPPINGS_H_
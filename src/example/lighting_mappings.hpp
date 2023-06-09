#ifndef _MINECPP_LIGHTING_MAPPINGS_H_
#define _MINECPP_LIGHTING_MAPPINGS_H_
#include <gl.h>
#include "../resource.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <imgui/imgui.h>

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

void init(Context& ctx){
   // imgui 准备阶段 : 
   // call ImGui::CreateContext()
   // call ImGui_ImplXXXX_Init() for each backend
   ImGui::CreateContext();
   // install_callbacks: 如果为true，则imgui会设置glfw的相关callback, 原来的callback（如果有）的函数指针会保存，imgui的callback会先调用
   ImGui_ImplGlfw_InitForOpenGL(ctx.getWindow(), true);
   std::string glslVersion = fmt::format("#version {}{}0", ctx.majorVersion, ctx.minorVersion);
   ImGui_ImplOpenGL3_Init(glslVersion.c_str());
}
void newFrame(){
   // frame 创建阶段: 
   // call ImGui_ImplXXXX_NewFrame() for each backend
   // call ImGui::NewFrame() 
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();
}
void render(){
   // render 阶段:  call ImGui::Render() and call ImGui_ImplXXXX_RenderDrawData() for Renderer backend
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void destroy(){
   // 销毁阶段: 
   // call ImGui_ImplXXXX_Shutdown() for each backend 
   // call ImGui::DestroyContext()
   ImGui_ImplGlfw_Shutdown();
   ImGui_ImplOpenGL3_Shutdown();
   ImGui::DestroyContext();
}

int run(){
   try{
      auto& ctx = Context::getInstance();
      ctx.createWindow(800, 600);
      auto& width = ctx.getWidth();
      auto& height = ctx.getHeight();
      Program cubeProgram {
         VertexShader::fromFile("../shader/lightingmapping/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/lightingmapping/cube.frag.glsl")
      };
      Program lightProgram {
         VertexShader::fromFile("../shader/lightingmapping/light.vertex.glsl"),
         FragmentShader::fromFile("../shader/lightingmapping/light.frag.glsl")
      };
      VertexBuffer vbo {vertices, sizeof(vertices), GL_STATIC_DRAW};
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


      DrawUnit cube  {vao, cubeProgram};
      DrawUnit light {vao, lightProgram};

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

      init(ctx);
      while (!glfwWindowShouldClose(ctx.getWindow()))
      {
         
         // 设置清除缓冲区的颜色并清除缓冲区
         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
         // 同时清除颜色缓冲区和深度缓冲区
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         cube.draw();
         glDrawArrays(GL_TRIANGLES, 0, 64);
         light.draw();
         glDrawArrays(GL_TRIANGLES, 0, 64);

         auto& lightPosVal = lightPos.value();
         newFrame();
         ImGui::Begin("controller");
         ImGui::SliderFloat("light position: x", &lightPosVal.x, -5.0f, 5.0f);
         ImGui::SliderFloat("light position: y", &lightPosVal.y, -5.0f, 5.0f);
         ImGui::SliderFloat("light position: z", &lightPosVal.z, -5.0f, 5.0f);
         ImGui::End();
         render();
         lightPos.check();

         // lightPos = glm::rotate(lightPos.value(), glm::radians(0.02f), glm::vec3(0.0f, 1.0f, 0.0f));
         // lightColor = glm::vec3(sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f));

         glfwSwapBuffers(ctx.getWindow());
         glfwPollEvents();
         checkGLError();
      }
      destroy();
      

   }catch(std::string e){
      fmt::println("{}", e);
   }
   return 0;
}

} // namespace lighting_mappings


#endif // _MINECPP_LIGHTING_MAPPINGS_H_
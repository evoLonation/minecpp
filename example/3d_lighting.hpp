#include <gl.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../render/render_template.hpp"
#include <array>
#include "../render/cube_normal.hpp"

namespace lighting3d {
using namespace render_template;

int run(){
   try{
      int width = 800;
      int height = 600;
      auto context = Context(width, height);
      context.addKeyHandler(GLFW_KEY_ESCAPE, GLFW_PRESS, 0, [](auto window){
         glfwSetWindowShouldClose(window, true);
      });
      context.addSizeChangeHandler([](auto window, auto width, auto height){
         glViewport(0, 0, width, height);
      });

      CubeNormal cube;

      Program cubeProgram = Program(
         VertexShader::fromFile("../shader/3d_lighting/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/3d_lighting/cube.frag.glsl")
      );
      Program lightProgram = Program(
         VertexShader::fromFile("../shader/3d_lighting/light.vertex.glsl"),
         FragmentShader::fromFile("../shader/3d_lighting/light.frag.glsl")
      );
      // model, view, projection 矩阵设置
      auto cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
      cubeModel = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * cubeModel;
      auto lightPos = glm::vec3(3.0f, 0.0f, -3.0f);
      auto lightModel = glm::scale(glm::vec3(0.2f, 0.2f, 0.2f));
      lightModel = glm::translate(lightModel, lightPos);
      glUseProgram(cubeProgram.getId());
      glUniformMatrix4fv(glGetUniformLocation(cubeProgram.getId(), "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
      glUseProgram(lightProgram.getId());
      glUniformMatrix4fv(glGetUniformLocation(lightProgram.getId(), "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
      auto viewPos = glm::vec3(1.0f, 0.0f, 1.0f);
      auto view = glm::lookAt(viewPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      glUseProgram(cubeProgram.getId());
      glUniformMatrix4fv(glGetUniformLocation(cubeProgram.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
      glUseProgram(lightProgram.getId());
      glUniformMatrix4fv(glGetUniformLocation(lightProgram.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
      auto projection = glm::perspective(glm::radians(45.0f), width * 1.0f / height , 0.1f, 100.0f);
      glUseProgram(cubeProgram.getId());
      glUniformMatrix4fv(glGetUniformLocation(cubeProgram.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
      glUseProgram(lightProgram.getId());
      glUniformMatrix4fv(glGetUniformLocation(lightProgram.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
      checkGLError();

      // 法向量变换矩阵的计算
      // 法向量不是位置，如果直接使用modol矩阵会添加位移
      // 直接使用model的3x3进行变换也不行，如果出现了非等比例的拉伸，向量就不会和片段垂直了
      // 因此原model矩阵取逆后转置，然后取3x3即可
      auto normalModel = glm::mat3(glm::transpose(glm::inverse(cubeModel)));
      glUseProgram(cubeProgram.getId());
      glUniformMatrix3fv(glGetUniformLocation(cubeProgram.getId(), "normalModel"), 1, GL_FALSE, glm::value_ptr(normalModel));

      auto cubeColor = glm::vec3(1.0f, 0.5f, 0.31f);
      auto lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

      // 设置cube program的 color, lightColor, lightPos, viewPos
      glUseProgram(cubeProgram.getId());
      glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "color"), 1, glm::value_ptr(cubeColor));
      glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "lightColor"), 1, glm::value_ptr(lightColor));
      glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "lightPos"), 1, glm::value_ptr(lightPos));
      glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "viewPos"), 1, glm::value_ptr(viewPos));
      // 设置light program的color
      glUseProgram(lightProgram.getId());
      glUniform3fv(glGetUniformLocation(lightProgram.getId(), "color"), 1, glm::value_ptr(lightColor));

      checkGLError();
      
      auto updateView = [&](const glm::mat4& view){
         glUseProgram(cubeProgram.getId());
         glUniformMatrix4fv(glGetUniformLocation(cubeProgram.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         // 同时改变viewPos，世界空间中的原点经过view变换为相机空间中的位置a，那么相机在世界中的位置就是-a
         glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "viewPos"), 1, glm::value_ptr(glm::vec3(glm::inverse(view)[3])));
         glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "viewPos"), 1, glm::value_ptr(glm::vec3(glm::inverse(view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
         glUseProgram(lightProgram.getId());
         glUniformMatrix4fv(glGetUniformLocation(lightProgram.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
      };
      context.addKeyHandler(GLFW_KEY_A, [&](auto window){
         view =  glm::translate(glm::vec3(0.1f, 0.0f, 0.0f)) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_D, [&](auto window){
         view =  glm::translate(glm::vec3(-0.1f, 0.0f, 0.0f)) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_Z, [&](auto window){
         view =  glm::translate(glm::vec3(0.0f, -0.1f, 0.0f)) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_X, [&](auto window){
         view =  glm::translate(glm::vec3(0.0f, 0.1f, 0.0f)) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_W, [&](auto window){
         view =  glm::translate(glm::vec3(0.0f, 0.0f, 0.1f)) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_S, [&](auto window){
         view =  glm::translate(glm::vec3(0.0f, 0.0f, -0.1f)) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_L, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_J, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f))) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_I, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f))) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_K, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f))) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_U, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f))) * view;
         updateView(view);
      });
      context.addKeyHandler(GLFW_KEY_O, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f))) * view;
         updateView(view);
      });

      context.startLoop([&](){
         glBindVertexArray(cube.getVertexArray().getId());
         glUseProgram(cubeProgram.getId());
         glDrawArrays(GL_TRIANGLES, 0, 64);
         checkGLError();
         glUseProgram(lightProgram.getId());
         glDrawArrays(GL_TRIANGLES, 0, 64);

         lightPos = glm::rotate(lightPos, glm::radians(0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
         lightModel = glm::translate(glm::mat4(1.0f), lightPos);
         lightModel = glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)) * lightModel;
         glUseProgram(cubeProgram.getId());
         glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "lightPos"), 1, glm::value_ptr(lightPos));
         glUseProgram(lightProgram.getId());
         glUniformMatrix4fv(glGetUniformLocation(lightProgram.getId(), "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
      
         // checkGLError();
      });

   }catch(std::string exp){
      std::cout << exp << std::endl;
      return 1;
   }
   return 0;
}

}


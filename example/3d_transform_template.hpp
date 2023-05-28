#include <gl.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../render/render_template.hpp"
#include "../render/render.hpp"
#include <array>

namespace transform3d_template {

using namespace render_template;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   if(action == GLFW_PRESS || action == GLFW_REPEAT){
      // shift + esc 退出
      if (key == GLFW_KEY_ESCAPE && (mods & GLFW_MOD_SHIFT))
      {
         // 设置应该退出窗口
         glfwSetWindowShouldClose(window, true);
      }
   }
}

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

      Cube cube;
      
      bool drawMulti = true;

      auto model = glm::mat4(1.0f);
      model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
      
      // 创建多个模型位置空间
      std::array<glm::vec3, 10> locations = {
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
      std::array<glm::mat4, 10> models;
      for(int i = 0; i < models.size(); i ++){
         models[i] = glm::translate(glm::mat4(1.0f), locations[i]);
      }

      auto view = glm::mat4(1.0f);
      view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

      auto projection = glm::perspective(glm::radians(45.0f), width * 1.0f / height , 0.1f, 100.0f);

      context.addSizeChangeHandler([&projection](auto window, auto width, auto height){
         projection = glm::perspective(glm::radians(45.0f), width * 1.0f / height , 0.1f, 100.0f);
      });

      Program program = Program(
         VertexShader::fromFile("../shader/3d_transform/vertex.glsl"),
         FragmentShader::fromFile("../shader/3d_transform/fragment.glsl"));
   
      glUseProgram(program.getId());

      // 通过设置着色器中类型为 sampler2D 的uniform变量的值来对应上下文中的纹理单元序号
      glUniform1i(glGetUniformLocation(program.getId(), "mTexture1"), 0);
      glUniform1i(glGetUniformLocation(program.getId(), "mTexture2"), 1);

      context.startLoop([&drawMulti, &program, &models, &view, &projection, &model](){
         glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         glUniformMatrix4fv(glGetUniformLocation(program.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
         
         if(drawMulti){
            for(auto model : models){
               glUniformMatrix4fv(glGetUniformLocation(program.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
               // 绘制。涉及到的上下文有vao、program、uniform、texture等等
               glDrawArrays(GL_TRIANGLES, 0, 64);
            }
         }else{
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
            // 绘制。涉及到的上下文有vao、program、uniform、texture等等
            glDrawArrays(GL_TRIANGLES, 0, 64);
         }
      });

   }catch(std::string exp){
      std::cout << exp << std::endl;
      return 1;
   }
   return 0;
}

}


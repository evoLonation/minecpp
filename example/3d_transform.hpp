#include <gl.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../render/render.hpp"
#include <array>

namespace transform3d {

using namespace render_old;

// action: GLFW_PRESS（下降沿触发）
//    GLFW_REPEAT（按下一会后的每个循环都会触发）
//    或者 GLFW_RELEASE（上升沿触发）
// key: GLFW_KEY_xxx，按键（包括特殊按键）
// scancode: 特定于平台的按键id，保证是唯一的
// mods: GLFW_MODE_xxx, 特殊按键，比如ctrl或者shift，不同的mod取或得到该值
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   // std::cout << "key:" << key << std::endl;
   // std::cout << "scancode:" << scancode << std::endl;
   // std::cout << "mods:" << mods << std::endl;
   // std::cout << "action:" << action << std::endl;
   if(action == GLFW_PRESS || action == GLFW_REPEAT){
      // shift + esc 退出
      if (key == GLFW_KEY_ESCAPE && (mods & GLFW_MOD_SHIFT))
      {
         // 设置应该退出窗口`
         glfwSetWindowShouldClose(window, true);
      }else if(key == GLFW_KEY_W){
      }else if(key == GLFW_KEY_S){
      }else if(key == GLFW_KEY_A){
      }else if(key == GLFW_KEY_D){
      }else if(key == GLFW_KEY_Q){
      }else if(key == GLFW_KEY_E){
      }else if(key == GLFW_KEY_Z){
      }else if(key == GLFW_KEY_X){
      }
   }
   
}

int run(){
   try{
      Context context = Context(1000, 1000);

      glfwSetKeyCallback(context.getWindow(), keyCallback);

      glEnable(GL_DEPTH_TEST);

      // Plane plane;
      Cube cube;
      
      bool drawMulti = true;

      // 从模型空间一直到相机空间，都假设是右手系

      // 创建一个单位矩阵作为模型矩阵
      // 矩阵的作用是将原坐标延x轴逆时针旋转55度（从负到正看轴）
      // 现在是世界空间的坐标
      // 那么即模型空间是相对于世界空间的x轴逆时针旋转55度得到的
      // 即：如果空间A相对于空间B做了变换T，那么如果想把空间A的坐标变换成空间B的坐标，就要对坐标进行T变换
      /***** model矩阵即是模型空间矩阵 *****/
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

      // 创建一个单位矩阵作为视图矩阵
      // 矩阵的作用是将原坐标延z轴移动-3个单位
      // 那么即世界空间是相对于相机空间的z轴移动-3个单位得到的
      // 即相机空间是相对于世界空间的z轴移动+3个单位得到的
      /***** view矩阵是相机空间逆矩阵 *****/
      auto view = glm::mat4(1.0f);
      view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

      // 创建一个投影矩阵
      // 从相机z轴负方向发出
      // 从一个投影空间变换为标准坐标空间
      // 由于标准设备坐标空间是左手系，因此将z轴倒置来换手
      // 这样本来离摄像机近的z值大，变成小
      auto projection = glm::perspective(glm::radians(45.0f), 1000.0f / 1000.0f, 0.1f, 100.0f);

      

      Program program = Program::fromFile(
         "../shader/3d_transform/vertex.glsl",
         "../shader/3d_transform/fragment.glsl");
   
      glUseProgram(program.getId());

      // 通过设置着色器中类型为 sampler2D 的uniform变量的值来对应上下文中的纹理单元序号
      glUniform1i(glGetUniformLocation(program.getId(), "mTexture1"), 0);
      glUniform1i(glGetUniformLocation(program.getId(), "mTexture2"), 1);
      

      // 当没有用户执行了窗口的退出操作后
      while (!glfwWindowShouldClose(context.getWindow()))
      {

         // 设置清除缓冲区的颜色并清除缓冲区
         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
         // 同时清除颜色缓冲区和深度缓冲区
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

         glfwSwapBuffers(context.getWindow());
         glfwPollEvents();
         checkGlError();
      }
      return 0;
   }catch(std::string exp){
      std::cout << exp << std::endl;
      return 1;
   }
}

}


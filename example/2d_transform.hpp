#include <gl.h>
#include <glfw3.h>
#include <iostream>
#include "../render/render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// 如果多个cpp文件中包含了该头文件，则必须有一个之前要加这个来包含实现
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace transform2d {

// 创建一个单位矩阵作为变换矩阵
glm::mat4 transform(1.0f);

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
         transform = glm::translate(transform, glm::vec3(0.0f, 0.1f, 0.0f));
      }else if(key == GLFW_KEY_S){
         transform = glm::translate(transform, glm::vec3(0.0f, -0.1f, 0.0f));
      }else if(key == GLFW_KEY_A){
         transform = glm::translate(transform, glm::vec3(-0.1f, 0.0f, 0.0f));
      }else if(key == GLFW_KEY_D){
         transform = glm::translate(transform, glm::vec3(0.1f, 0.0f, 0.0f));
      }else if(key == GLFW_KEY_Q){
         transform = glm::rotate(transform, glm::radians(10.0f), glm::vec3(0.f, 0.f, 1.0f));
      }else if(key == GLFW_KEY_E){
         transform = glm::rotate(transform, glm::radians(-10.0f), glm::vec3(0.f, 0.f, 1.0f));
      }else if(key == GLFW_KEY_Z){
         transform = glm::scale(transform, glm::vec3(0.9f, 0.9f, 0.9f));
      }else if(key == GLFW_KEY_X){
         transform = glm::scale(transform, glm::vec3(1/0.9f, 1/0.9f, 1/0.9f));
      }
   }
   
}

bool run()
{
   GLFWwindow* window = initialize(1000, 1000);
   if(window == nullptr){
      return false;
   }

   glfwSetKeyCallback(window, keyCallback);

   if(!createPlane()){
      glfwTerminate();
      return false;
   }

   // 启用着色器
   GLuint program = createLinkedProgram("../shader/2d_transform/vertex.glsl", "../shader/2d_transform/fragment.glsl");
   if(program == 0){
      deletePlane();
      glfwTerminate();
      return false;
   }
   
   glUseProgram(program);

   // 当没有用户执行了窗口的退出操作后
   while (!glfwWindowShouldClose(window))
   {

      // 设置清除缓冲区的颜色并清除缓冲区
      // 设置状态
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      // 使用状态
      glClear(GL_COLOR_BUFFER_BIT);

      // 线框模式打开（在三角形的前面和后面都使用线框模式）
      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      // 填充模式（默认模式）打开（在三角形的前面和后面都使用线框模式）
      // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      // 开始渲染，绘制三角形，偏移为0，顶点数量为3（3/3=1个三角形）
      // glDrawArrays(GL_TRIANGLES, 0, 3);

      // GLfloat timeValue = glfwGetTime();
      // GLfloat greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
      // // 设置uniform变量的值
      // GLint uniformLocation = glGetUniformLocation(program, "color");
      // // 需要提前设置使用的program
      // glUniform4f(uniformLocation, 0.0f, greenValue, 0.0f, 1.0f);
      
      // opengl 采用column-major ordering（列主排序）的矩阵存储方式，从上往下再从左往右
      glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

      // 通过设置着色器中类型为 sampler2D 的uniform变量的值来对应上下文中的纹理单元序号
      GLint textLocation1 = glGetUniformLocation(program, "mTexture1");
      glUniform1i(textLocation1, 0);
      GLint textLocation2 = glGetUniformLocation(program, "mTexture2");
      glUniform1i(textLocation2, 1);

      // 开始渲染，绘制三角形，索引数量为6（6/3=2个三角形），偏移为0（如果vao上下文没有绑定ebo则为数据的内存指针）
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      // 切换渲染的缓冲区将其显示到屏幕上输出
      glfwSwapBuffers(window);
      // 使用之前注册的回调函数处理所有事件
      glfwPollEvents();
      if(checkGlError()){
         glDeleteProgram(program);
         deletePlane();
         glfwTerminate();
         return false;
      }
   }

   glDeleteProgram(program);
   deletePlane();

   // 销毁glfw的所有资源，退出前调用
   glfwTerminate();
   return true;
}

}


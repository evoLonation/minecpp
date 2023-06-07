#ifndef _MINECPP_3D_TRANSFORM_TEMPLATE_H_
#define _MINECPP_3D_TRANSFORM_TEMPLATE_H_

#include <gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../render_template.hpp"
#include "../render.hpp"
#include <array>

namespace transform3d_template {
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

      Cube cube;
      Program program = Program(
         VertexShader::fromFile("../shader/3d_transform/vertex.glsl"),
         FragmentShader::fromFile("../shader/3d_transform/fragment.glsl"));
      glUseProgram(program.getId());
      
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

      // 从相机的方向上看过去，其右边是x轴正方向，上边是y轴正方向，前边是z轴反方向
      auto position = glm::vec3(0.0f, 0.0f, 3.0f);
      auto front = glm::vec3(0.0f, 0.0f, -1.0f);
      auto up = glm::vec3(0.0f, 1.0f, 0.0f);
      // 使用glm::lookAt方法，给定相机位置、目标（相机对准的地方，z轴反方向）与y轴方向（都是相对于世界坐标系），得到相机的view矩阵
      auto view = glm::lookAt(position, position + front, up);
      glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));

      int apporach = 1;
      if(apporach == 0){
         // 第一种方法
         // 当移动时，先改变对应的向量，然后再重新计算一次view矩阵
         // 对于平移，可以根据平移的方向得到一个delta然后加到position中
         // 对于旋转，先判断是绕哪个轴旋转，然后改变对应的其他方向向量（使用glm::rotate）

         // 注意！speed和changeView的生命周期在该if中，因此后面的lambda要保证传入副本而不是引用
         const float speed = 0.1;
         auto changeView = [&program, &view, &position, &front, &up](){
            view = glm::lookAt(position, position + front, up);
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         };
         // 相机上下移动
         context.addKeyHandler(GLFW_KEY_Z, [changeView, speed, &position, &up](auto window){
            position += speed * up;
            changeView();
         });
         context.addKeyHandler(GLFW_KEY_X, [changeView, speed, &position, &up](auto window){
            position -= speed * up;
            changeView();
         });
         // 相机左右移动
         context.addKeyHandler(GLFW_KEY_A, [changeView, speed, &position, &front, &up](auto window){
            // 前与上创建右向量（右手法则）
            position -= speed * glm::normalize(glm::cross(front, up));
            changeView();
         });
         context.addKeyHandler(GLFW_KEY_D, [changeView, speed, &position, &front, &up](auto window){
            position += speed * glm::normalize(glm::cross(front, up));
            changeView();
         });
         // 相机前后移动
         context.addKeyHandler(GLFW_KEY_W, [changeView, speed, &position, &front](auto window){
            position += speed * front;
            changeView();
         });
         context.addKeyHandler(GLFW_KEY_S, [changeView, speed, &position, &front](auto window){
            position -= speed * front;
            changeView();
         });
         // 相机左右旋转
         context.addKeyHandler(GLFW_KEY_J, [changeView, &front, &up](auto window){
            // 向左转，从上往下是front在逆时针转，从底往上就是顺时针
            front = glm::rotate(front, glm::radians(1.0f), up);
            changeView();
         });
         context.addKeyHandler(GLFW_KEY_L, [changeView, &front, &up](auto window){
            front = glm::rotate(front, glm::radians(-1.0f), up);
            changeView();
         });
         // 相机沿front旋转（歪着看）
         context.addKeyHandler(GLFW_KEY_U, [changeView, &front, &up](auto window){
            // 向左歪头，就是up逆时针转
            up = glm::rotate(up, glm::radians(-1.0f), front);
            changeView();
         });
         context.addKeyHandler(GLFW_KEY_O, [changeView, &front, &up](auto window){
            up = glm::rotate(up, glm::radians(1.0f), front);
            changeView();
         });
         // 相机上下旋转
         // front和up都要沿着right转
         context.addKeyHandler(GLFW_KEY_I, [changeView, &front, &up](auto window){
            auto right = glm::normalize(glm::cross(front, up));
            up = glm::rotate(up, glm::radians(1.0f), right);
            front = glm::rotate(front, glm::radians(1.0f), right);
            changeView();
         });
         context.addKeyHandler(GLFW_KEY_K, [changeView, &front, &up](auto window){
            auto right = glm::normalize(glm::cross(front, up));
            up = glm::rotate(up, glm::radians(-1.0f), right);
            front = glm::rotate(front, glm::radians(-1.0f), right);
            changeView();
         });
      }else if(apporach == 1){
         // 对于方法1，你需要明确：view矩阵是相机空间的逆矩阵
         // 并且，一个空间矩阵左乘一个变换矩阵，等价于变换到一个新的空间
         // 因此，变换矩阵应该也是实际相机变换的逆矩阵
         // 其实可以这样看：view是世界相对于相机的矩阵，我们需要的是整个世界矩阵的变换
         // 比如相机左移，其实就是世界空间右移
         // 然后旋转的话是通过计算四元数然后转换为变换矩阵

         // 左右移动
         context.addKeyHandler(GLFW_KEY_A, [&view, &program](auto window){
            view =  glm::translate(glm::vec3(0.1f, 0.0f, 0.0f)) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         context.addKeyHandler(GLFW_KEY_D, [&view, &program](auto window){
            view =  glm::translate(glm::vec3(-0.1f, 0.0f, 0.0f)) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         // 上下移动
         context.addKeyHandler(GLFW_KEY_Z, [&view, &program](auto window){
            view =  glm::translate(glm::vec3(0.0f, -0.1f, 0.0f)) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         context.addKeyHandler(GLFW_KEY_X, [&view, &program](auto window){
            view =  glm::translate(glm::vec3(0.0f, 0.1f, 0.0f)) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         // 前后移动
         context.addKeyHandler(GLFW_KEY_W, [&view, &program](auto window){
            view =  glm::translate(glm::vec3(0.0f, 0.0f, 0.1f)) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         context.addKeyHandler(GLFW_KEY_S, [&view, &program](auto window){
            view =  glm::translate(glm::vec3(0.0f, 0.0f, -0.1f)) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });

         // glm::angleAxis函数通过输入旋转的角度与旋转轴来创建四元数；
         // 然后glm::mat4_cast输入四元数后创建对应的变换矩阵。

         // 相机左右旋转
         context.addKeyHandler(GLFW_KEY_L, [&view, &program](auto window){
            view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         context.addKeyHandler(GLFW_KEY_J, [&view, &program](auto window){
            view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f))) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         // 相机上下旋转
         context.addKeyHandler(GLFW_KEY_I, [&view, &program](auto window){
            view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f))) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         context.addKeyHandler(GLFW_KEY_K, [&view, &program](auto window){
            view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f))) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         // 相机顺逆时针旋转
         context.addKeyHandler(GLFW_KEY_U, [&view, &program](auto window){
            view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f))) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         context.addKeyHandler(GLFW_KEY_O, [&view, &program](auto window){
            view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f))) * view;
            glUniformMatrix4fv(glGetUniformLocation(program.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         });
         
      }
      
      
      
      
      auto projection = glm::perspective(glm::radians(45.0f), width * 1.0f / height , 0.1f, 100.0f);
      glUniformMatrix4fv(glGetUniformLocation(program.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

      // 当窗口尺寸变化时，修改投影矩阵的fov使得匹配屏幕的宽高比例
      context.addSizeChangeHandler([&projection, &program](auto window, auto width, auto height){
         projection = glm::perspective(glm::radians(45.0f), width * 1.0f / height , 0.1f, 100.0f);
         glUniformMatrix4fv(glGetUniformLocation(program.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
      });

      glUniform1i(glGetUniformLocation(program.getId(), "mTexture1"), 0);
      glUniform1i(glGetUniformLocation(program.getId(), "mTexture2"), 1);

      context.startLoop([&program, &models, &view, &projection](){         
         for(auto model : models){
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


#endif // _MINECPP_3D_TRANSFORM_TEMPLATE_H_
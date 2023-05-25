#include <gl.h>
#include <glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include "render/render.hpp"
#include <cmath>
// 如果多个cpp文件中包含了该头文件，则必须有一个之前要加这个来包含实现
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLfloat factor = 1.0f;

// 当窗口变化时调用的回调函数
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
   glViewport(0, 0, width, height);
}

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
      }else if(key == GLFW_KEY_UP || key == GLFW_KEY_W){
         factor += 0.1;
      }else if(key == GLFW_KEY_DOWN || key == GLFW_KEY_S){
         factor -= 0.1;
      }else if(key == GLFW_KEY_A || key == GLFW_KEY_D){
         glActiveTexture(GL_TEXTURE0);
         static int i = 0;
         printf("i: %d\n", i);
         GLenum param;
         if(key == GLFW_KEY_A){
            param = GL_TEXTURE_MIN_FILTER;
         }else{
            param = GL_TEXTURE_MAG_FILTER;
         }
         switch(i){
         case 0: 
            glTexParameteri(GL_TEXTURE_2D, param, GL_NEAREST); 
            i++; break;
         case 1:
            glTexParameteri(GL_TEXTURE_2D, param, GL_LINEAR);
            i++; break;
         case 2:
            glTexParameteri(GL_TEXTURE_2D, param, GL_NEAREST_MIPMAP_LINEAR);
            i++; break;
         case 3:
            glTexParameteri(GL_TEXTURE_2D, param, GL_NEAREST_MIPMAP_NEAREST);
            i++; break;
         case 4:
            glTexParameteri(GL_TEXTURE_2D, param, GL_LINEAR_MIPMAP_LINEAR);
            i++; break;
         case 5:
            glTexParameteri(GL_TEXTURE_2D, param, GL_LINEAR_MIPMAP_NEAREST);
            i=0; break;  
         }
      }
   }
   
}

int main()
{
   GLFWwindow* window = initialize();

   // 设置当窗口尺寸变化时的回调函数
   // 还有很多的回调函数，如处理输入等等；须在创建窗口后、开始渲染前注册回调函数
   glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

   glfwSetKeyCallback(window, keyCallback);

   
   
   GLuint texture1 = createTexture(0, "../container.jpg");
   GLuint texture2 = createTexture(1, "../awesome-face.png");
   
   GLfloat vertices3[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   3.0f, 3.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   3.0f, -2.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   -2.0f, -2.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   -2.0f, 3.0f    // top left 
   };

   // 准备渲染
   GLfloat vertices[] = {
       -0.5f, -0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.0f, 0.5f, 0.0f,
   };
   GLfloat vertices4[] = {
       -0.5f, -0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.5f, 0.5f, 0.0f,
       -0.5f, 0.5f, 0.0f,
   };
   GLfloat colors4[] = {
       1.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
   };
   GLfloat verticesColor4[] = {
       -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
       0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
       -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
   };
   GLuint index4[] = {
      0, 1, 2, 
      0, 2, 3,
   };

   // 创建一个vao对象
   GLuint vao;
   glGenVertexArrays(1, &vao);
   // 设置vao上下文为刚刚创建的对象，之后会在vao上下文中存储顶点属性和EBO的设置（不直接存储vbo的状态）
   glBindVertexArray(vao);

   GLuint vbo = createBuffer(GL_ARRAY_BUFFER, vertices3, sizeof(vertices3), GL_STATIC_DRAW);
   // 该函数将当前位于GL_ARRAY_BUFFER上下文的buffer对象的设置为顶点属性
   // index: 对应着色器中的location
   // size: 对应单个属性的元素的个数，比如vec3对应3
   // type: 数据的元素的类型，一般来说和属性的类型保持一致（如果不一致则会进行类型转换），如vec3的基本类型是float，则对应GL_FLOAT
   // normalized: 如果设置为GL_TRUE，那么如果输入的值的类型是整形且对应着色器属性的类型是浮点型，就会映射到[-1, 1]（无符号则为[0, 1]）
   // stride: 步长，多少字节拿一次数据
   // offset：从所给数组的那里开始拿数据
   // 注：这里如果和着色器的类型没有完全对应其实也有可能正常运行
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
   // 启用index对应的顶点属性
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);

   GLuint ebo = createBuffer(GL_ELEMENT_ARRAY_BUFFER, index4, sizeof(index4), GL_STATIC_DRAW);

   // 启用着色器
   GLuint program = createLinkedProgram("../shader/vertex.glsl", "../shader/fragment.glsl");
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
      
      glUniform1f(glGetUniformLocation(program, "factor"), factor);

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

      // 睡眠500毫秒
      // std::chrono::milliseconds duration(500);
      // std::this_thread::sleep_for(duration);
   }

   
   // 销毁glfw的所有资源，退出前调用
   glfwTerminate();

   

   return 0;
}

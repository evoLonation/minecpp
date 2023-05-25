#include <gl.h>
#include <glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include "render/render.hpp"
#include <cmath>

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
   std::cout << "key:" << key << std::endl;
   std::cout << "scancode:" << scancode << std::endl;
   std::cout << "mods:" << mods << std::endl;
   std::cout << "action:" << action << std::endl;
   // shift + esc 退出
   if (key == GLFW_KEY_ESCAPE && (mods & GLFW_MOD_SHIFT))
   {
      // 设置应该退出窗口`
      glfwSetWindowShouldClose(window, true);
   }
}

int main()
{
   glfwInit();
   // 设置opengl版本3.3
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   // 设置openglapi类型为core profile
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   // 创建窗口
   GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
   if (window == NULL)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }
   // 将创建的窗口设置为当前opengl上下文
   glfwMakeContextCurrent(window);

   // 借助 glad加载opengl的函数
   int version = gladLoadGL(glfwGetProcAddress);
   if (version == 0)
   {
      printf("Failed to initialize GLAD\n");
      return -1;
   }
   std::cout << "Loaded OpenGL" << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

   // 设置opengl渲染在窗口中的起始位置和大小
   glViewport(0, 0, 800, 600);

   // 设置当窗口尺寸变化时的回调函数
   // 还有很多的回调函数，如处理输入等等；须在创建窗口后、开始渲染前注册回调函数
   glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

   glfwSetKeyCallback(window, keyCallback);


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

   GLuint vbo3 = createBuffer(GL_ARRAY_BUFFER, verticesColor4, sizeof(verticesColor4), GL_STATIC_DRAW);
   
   GLuint vbo = createBuffer(GL_ARRAY_BUFFER, vertices4, sizeof(vertices4), GL_STATIC_DRAW);
   // 该函数将当前位于GL_ARRAY_BUFFER上下文的buffer对象的设置为顶点属性
   // index: 对应着色器中的location
   // size: 对应单个属性的元素的个数，比如vec3对应3
   // type: 数据的元素的类型，一般来说和属性的类型保持一致（如果不一致则会进行类型转换），如vec3的基本类型是float，则对应GL_FLOAT
   // normalized: 如果设置为GL_TRUE，那么如果输入的值的类型是整形且对应着色器属性的类型是浮点型，就会映射到[-1, 1]（无符号则为[0, 1]）
   // stride: 步长，多少字节拿一次数据
   // offset：从所给数组的那里开始拿数据
   // 注：这里如果和着色器的类型没有完全对应其实也有可能正常运行
   // glBindBuffer(GL_ARRAY_BUFFER, vbo3);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
   // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
   // 启用index对应的顶点属性
   glEnableVertexAttribArray(0);

   GLuint vbo2 = createBuffer(GL_ARRAY_BUFFER, colors4, sizeof(colors4), GL_STATIC_DRAW);

   // glBindBuffer(GL_ARRAY_BUFFER, vbo3);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
   // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
   glEnableVertexAttribArray(1);

   GLuint ebo = createBuffer(GL_ELEMENT_ARRAY_BUFFER, index4, sizeof(index4), GL_STATIC_DRAW);

   // 启用着色器
   GLuint program = createLinkedProgram("shader/vertex.glsl", "shader/fragment.glsl");
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

      GLfloat timeValue = glfwGetTime();
      GLfloat greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
      // 设置uniform变量的值
      GLint uniformLocation = glGetUniformLocation(program, "color");
      // 需要提前设置使用的program
      glUniform4f(uniformLocation, 0.0f, greenValue, 0.0f, 1.0f);
      
      

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

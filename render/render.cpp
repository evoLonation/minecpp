#include "render.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stb_image.h>

GLFWwindow* initialize(){
   glfwInit();
   // 设置opengl版本3.3,类型为core profile
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   // 创建窗口
   GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
   if (window == NULL)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return NULL;
   }
   // 将创建的窗口设置为当前opengl上下文
   glfwMakeContextCurrent(window);

   // 借助 glad加载opengl的函数
   int version = gladLoadGL(glfwGetProcAddress);
   if (version == 0)
   {
      printf("Failed to initialize GLAD\n");
      return NULL;
   }
   std::cout << "Loaded OpenGL" << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

   // 设置opengl渲染在窗口中的起始位置和大小
   glViewport(0, 0, 800, 600);
   return window;
}

// 创建一个指定类型的buffer并设置其为当前上下文，同时绑定数据
GLuint createBuffer(GLenum bufferType, const void* data, GLsizeiptr size, GLenum usage){
   // 创建一个buffer对象
   GLuint buffer;
   glGenBuffers(1, &buffer);
   // 将创建的buffer对象设置到GL_ARRAY_BUFFER上下文中
   glBindBuffer(bufferType, buffer);
   // 设置GL_ARRAY_BUFFER上下文对象的数据
   // GL_STREAM_DRAW：数据只设置一次，GPU最多使用几次
   // GL_STATIC_DRAW：数据只设置一次，使用多次
   // GL_DYNAMIC_DRAW：数据变化很大，使用次数也很多
   glBufferData(bufferType, size, data, usage);
   return buffer;
}

GLuint createCompiledShader(GLenum type, const char* path)
{

   // 读取着色器文件
   std::ifstream shaderFile(path);
   if (!shaderFile.is_open())
   {
      std::cout << "Open shader file "
                << path
                << "failed!" << std::endl;
      return 0;
   }
   std::stringstream sstream;
   sstream << shaderFile.rdbuf();
   std::string content = sstream.str();
   const char *shaderContent = content.c_str();

   // 创建着色器对象
   GLuint shader = glCreateShader(type);
   // 第二个参数是字符串的数量
   glShaderSource(shader, 1, &shaderContent, nullptr);
   glCompileShader(shader);

   // 判断编译是否错误
   GLint status;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
   if (status == GL_FALSE)
   {
      GLint logLength;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

      GLchar logInfo[logLength + 1];
      glGetShaderInfoLog(shader, logLength, nullptr, logInfo);

      std::cout << "compile shader failure: " << logInfo;
      return 0;
   }
   return shader;
}

GLuint createLinkedProgram(const char* vertexShaderPath, const char* fragmentShaderPath){
   GLuint vertexShader = createCompiledShader(GL_VERTEX_SHADER, vertexShaderPath);
   GLuint fragmentShader = createCompiledShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
   
   // 创建program对象
   GLuint program = glCreateProgram();
   
   // 将shader加入program
   glAttachShader(program, vertexShader); 
   glAttachShader(program, fragmentShader);

   // 链接program
   glLinkProgram(program);


   // 判断编译是否错误
   GLint status;
   glGetProgramiv(program, GL_LINK_STATUS, &status);
   if (status == GL_FALSE)
   {
      GLint logLength;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

      GLchar logInfo[logLength + 1];
      glGetProgramInfoLog(program, logLength, nullptr, logInfo);

      std::cout << "link program failure: " << logInfo;
      return 0;
   }

   // 链接成功后就可以删除着色器对象了
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
   return program;
}

GLuint createTexture(GLint unitId, const char* filepath){
   // 默认的unit就是GL_TEXTURE0
   glActiveTexture(GL_TEXTURE0 + unitId);
   // 创建texture对象并将其绑定到当前texture unit的GL_TEXTURE_2D上下文中
   // 一个texture unit 只能对应于着色器中的一个纹理属性（也即一个unit只需要设置一个类型的上下文，设置多个是没有意义的）
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   // 设置纹理的参数
   // GL_TEXTURE_WRAP_S: 横向；GL_TEXTURE_WRAP_T：纵向；
   // 过滤方式：GL_TEXTURE_MIN_FILTER: 纹理缩小（图像离得很远）；GL_TEXTURE_MAG_FILTER: 纹理放大（图像离得很近）；
   // 对于GL_TEXTURE_MAG_FILTER，他只能设置GL_NEAREST和GL_LINEAR两种，因为肯定会使用第一级别的mipmap
   // 如果GL_TEXTURE_MIN_FILTER的过滤方式设置为和MipMap相关的，那么在加载纹理数据后需要使用glGenerateMipmap生成mipmap
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   
   // 设置GL_CLAMP_TO_EDGE参数后，需要额外自定义颜色
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
   // GLfloat borderColor[] = { 0.3f, 0.7f, 0.2f, 1.0f };
   // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
   
   int width, height, nrChannels;
   // opengl的纹理坐标的y轴是从底向上从0到1的，但是stb默认加载是从上向下加载的，因此需要翻转一下y轴
   stbi_set_flip_vertically_on_load(true);
   // 读取图片并返回数据、图片的宽高和通道数
   unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
   if(data == nullptr){
      printf("load image from %s error\n", filepath);
      return 0;
   }
   printf("the channel number of image %s : %d\n", filepath, nrChannels);
   if(nrChannels == 3){
      // 源数据的通道数为3，说明类型是rgb
      // jpg格式通常是rgb

      // 加载纹理数据
      // target: 要设置的上下文
      // level: mipmap 等级
      // internalFormat: 内部存储纹理的格式
      // width、height：图片的宽高
      // border：总为0
      // format, type：输入数据的纹理格式
      // pixels：数据
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   }else if(nrChannels == 4){
      // 源数据的通道数为4，说明类型是rgba
      // png格式通常是rgba
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
   }else{
      stbi_image_free(data);
      return 0;
   }
   glGenerateMipmap(GL_TEXTURE_2D);
   
   // 释放读取图像的内存
   stbi_image_free(data);
   return texture;
}
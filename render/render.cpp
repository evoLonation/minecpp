#include "render.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

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

GLuint createCompiledShader(GLenum type, const std::string& path)
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

GLuint createLinkedProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath){
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
#ifndef RENDER_TEMPLATE_HPP
#define RENDER_TEMPLATE_HPP

#include <gl.h>
#include <glfw3.h>
#include <optional>
#include <string>
#include <fmt/format.h>
#include <functional>
#include <fstream>
#include <sstream>
#include "../exception.hpp"
#include <stb_image.h>

namespace render_template{


enum ResourceType{
   BUFFER, VERTEXARRAY, TEXTURE, PROGRAM, SHADER
};

template<ResourceType type, GLenum subType=0>
class GLResource{
private:
   //待特化
   void deleteResource(GLuint id);
   GLuint createResource();
protected:
   GLuint id;
   GLResource() = default;
public:

   GLResource(GLResource&& resource):id(resource.id){resource.id = 0;}
   
   GLResource(const GLResource& resource) = delete;
   GLResource& operator=(const GLResource& resource) = delete;
   GLResource& operator=(GLResource&& resource) = delete;
   ~GLResource(){if(this->id != 0){deleteResource(this->id);}}
   
   GLuint getId()const{return id;}
};

// 特化的类模板部分
template <>
void GLResource<BUFFER>::deleteResource(GLuint id) {
   glDeleteBuffers(1, &id);
}
template <>
void GLResource<VERTEXARRAY>::deleteResource(GLuint id) {
   glDeleteVertexArrays(1, &id);
}
template <>
void GLResource<TEXTURE>::deleteResource(GLuint id) {
   glDeleteTextures(1, &id);
}
template <>
void GLResource<PROGRAM>::deleteResource(GLuint id) {
   glDeleteProgram(id);
}
template <>
void GLResource<SHADER>::deleteResource(GLuint id) {
   glDeleteShader(id);
}
template <>
GLuint GLResource<BUFFER>::createResource() {
   GLuint id;
   glGenBuffers(1, &id);
   return id;
}
template <>
GLuint GLResource<VERTEXARRAY>::createResource() {
   GLuint id;
   glGenVertexArrays(1, &id);
   return id;
}
template <>
GLuint GLResource<TEXTURE>::createResource() {
   GLuint id;
   glGenTextures(1, &id);
   return id;
}
template <>
GLuint GLResource<PROGRAM>::createResource() {
   return glCreateProgram();
}
template <>
GLuint GLResource<SHADER, GL_VERTEX_SHADER>::createResource() {
   return glCreateShader(GL_VERTEX_SHADER);
}
template <>
GLuint GLResource<SHADER, GL_FRAGMENT_SHADER>::createResource() {
   return glCreateShader(GL_FRAGMENT_SHADER);
}

// template<ResourceType type, GLenum subType = 0>
// class ContextTarget{
// private:
//    // 等待特化
//    void bind(GLuint id);
//    GLuint target;
// public:
//    bool isContext(const GLResource<type>& object) const {
//       return object.getId() == target;
//    }
//    void bindContext(const GLResource<type>& object){
//       if(!isContext(object)) bind(object.getId());
//    }
// };

// // 特化的类模板部分
// template <>
// void ContextTarget<BUFFER, GL_ELEMENT_ARRAY_BUFFER>::bind(GLuint id) {
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
// }
// template <>
// void ContextTarget<BUFFER, GL_ARRAY_BUFFER>::bind(GLuint id) {
//    glBindBuffer(GL_ARRAY_BUFFER, id);
// }
// template <>
// void ContextTarget<VERTEXARRAY>::bind(GLuint id) {
//    glBindVertexArray(id);
// }
// template <>
// void ContextTarget<PROGRAM>::bind(GLuint id) {
//    glUseProgram(id);
// }
// template <>
// void ContextTarget<TEXTURE, GL_TEXTURE_2D>::bind(GLuint id) {
//    glBindTexture(GL_TEXTURE_2D, id);
// }

// using VertexBufferContext = ContextTarget<BUFFER, GL_ARRAY_BUFFER>;
// using ElementBufferContext = ContextTarget<BUFFER, GL_ELEMENT_ARRAY_BUFFER>;
// using VertexArrayContext = ContextTarget<VERTEXARRAY>;
// using ProgramContext = ContextTarget<PROGRAM>;
// using TextureContext = ContextTarget<TEXTURE, GL_TEXTURE_2D>;


template<GLenum bufferType>
class Buffer: public GLResource<BUFFER>{
public:
   Buffer(const void* data, GLsizeiptr size, GLenum usage);
};

template<GLenum bufferType>
// 创建一个指定类型的buffer并设置其为当前上下文，同时绑定数据
Buffer<bufferType>::Buffer(const void* data, GLsizeiptr size, GLenum usage)
{
   // 将创建的buffer对象设置到GL_ARRAY_BUFFER上下文中
   glBindBuffer(bufferType, this->id);
   glBufferData(bufferType, size, data, usage);
   // todo check error
}

using VertexBuffer = Buffer<GL_ARRAY_BUFFER>;
using ElementBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER>;

class VertexArray: public GLResource<VERTEXARRAY>{
public:
   VertexArray() = default;
   void addAttribute(
   const VertexBuffer& buffer,
   GLuint index, 
   GLint size, 
   GLenum type, 
   GLboolean normalized, 
   GLsizei stride, 
   const void * pointer){
      glBindVertexArray(this->id);
      glBindBuffer(GL_ARRAY_BUFFER, buffer.getId());
      glVertexAttribPointer(index, size, type, normalized, stride, pointer);
      glEnableVertexAttribArray(index);
   }
   void bindElemenBuffer(const ElementBuffer& buffer){
      glBindVertexArray(this->id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.getId());
   }
};

template<GLenum shaderType>
class Shader: public GLResource<SHADER, shaderType>{
   Shader(const std::string& str, bool isContent);
   static Shader fromFile(GLenum type, const std::string& path){return Shader(path, false);};
   static Shader fromContent(GLenum type, const std::string& content){return Shader(content, true);};
};
using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

template<GLenum shaderType>
Shader<shaderType>::Shader(const std::string& str, bool isContent){
   GLuint shader = this->id;
   std::string content;
   if(!isContent){
      // 读取着色器文件
      std::ifstream shaderFile = std::ifstream(str);
      if (!shaderFile.is_open())
      {
         error(fmt::format("Open shader file {} failed!", str));
      }
      std::stringstream sstream;
      sstream << shaderFile.rdbuf();
      content = std::move(sstream.str());
   }else{
      content = std::move(str);
   }
   // 第二个参数是字符串的数量
   const char* c_content = content.c_str();
   glShaderSource(shader, 1, &c_content, nullptr);
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
      
      glDeleteShader(shader);
      error(fmt::format("compile shader failure: {}", (char*)logInfo));
   }
}

class Program: public GLResource<PROGRAM>{
public:
   Program(VertexShader&& vertexShader, FragmentShader&& fragmentShader);
};

Program::Program(VertexShader&& vertexShader, FragmentShader&& fragmentShader) {
   GLuint program = this->id;

   glAttachShader(program, vertexShader.getId()); 
   glAttachShader(program, fragmentShader.getId());

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

      glDeleteProgram(program);
      error(fmt::format("link program failure: {}", (char*)logInfo));
   }
   
}
template<GLenum textureType>
class Texture: public GLResource<TEXTURE>{
protected:
   Texture() = default;
public:
   void bindUnit(int unit){
      glActiveTexture(GL_TEXTURE0 + unit);
      glBindTexture(textureType, this->id);
   }
};

class Texture2D: public Texture<GL_TEXTURE_2D>{
public:
   Texture2D(const char* filepath): Texture2D(filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr) {}
   Texture2D(const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor);
   void bindUnit(int unit);
};

Texture2D::Texture2D(const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor){
   GLuint texture = this->id;
   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizonType);	
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalType);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterType);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterType);

   if(horizonType == GL_CLAMP_TO_BORDER || verticalType == GL_CLAMP_TO_BORDER){
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
   }
   
   int width, height, nrChannels;
   stbi_set_flip_vertically_on_load(true);
   unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
   if(data == nullptr){
      glDeleteTextures(1, &texture);
      error(fmt::format("load image from {} failed", filepath));
   }
   fmt::print("the channel number of image {} : {}\n", filepath, nrChannels);
   if(nrChannels == 3){
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   }else if(nrChannels == 4){
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
   }else{
      glDeleteTextures(1, &texture);
      stbi_image_free(data);
      error(fmt::format("the channel of image is not 3 nor 4, is {}", nrChannels));
   }

   stbi_image_free(data);
   
   if(   magFilterType == GL_LINEAR_MIPMAP_LINEAR ||
         magFilterType == GL_LINEAR_MIPMAP_NEAREST ||
         magFilterType == GL_NEAREST_MIPMAP_LINEAR ||
         magFilterType == GL_NEAREST_MIPMAP_NEAREST) {
      glGenerateMipmap(GL_TEXTURE_2D);
   }

}

};


#endif
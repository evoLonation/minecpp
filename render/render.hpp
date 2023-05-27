#ifndef RENDER_HPP
#define RENDER_HPP

#include <gl.h>
#include <glfw3.h>
#include <optional>
#include <string>
#include <fmt/format.h>
#include <functional>

// 定义一个GL资源类，用于表示一些opengl中buffer、shader、program、texture资源的共性
// 所有资源都没有赋值操作和拷贝构造操作
// 具有移动构造函数
class GLResource{
private:
   std::function<void(GLuint)> deleteFunc;
protected:
   // 通过glCreate...创建id
   GLuint id;
public:
   // 构造该类需要提供一个删除函数，其接收一个不为0的id，执行对应资源的删除操作
   GLResource(const std::function<void(GLuint)>& deleteFunc):deleteFunc(deleteFunc){}

   // 移动构造，将id移动到对面，自身设置为0，析构时判断是否为0来判断是否需要调用释放函数
   // 同时移动deletFunc
   GLResource(GLResource&& resource)
   :deleteFunc(std::move(resource.deleteFunc)),
   id(resource.id){resource.id = 0;}
   
   GLResource(const GLResource& resource) = delete;
   GLResource& operator=(const GLResource& resource) = delete;
   GLResource& operator=(GLResource&& resource) = delete;
   ~GLResource(){if(this->id != 0){deleteFunc(this->id);}}
   
   GLuint getId(){return id;}
};

// 用于管理一个opengl的buffer对象的资源
class Buffer: public GLResource{
public:
   Buffer(GLenum bufferType, const void* data, GLsizeiptr size, GLenum usage);
};
class Shader: public GLResource{
public:
   Shader(GLenum type, const std::string& str, bool isContent);
   inline static Shader fromFile(GLenum type, const std::string& path){return Shader(type, path, false);};
   inline static Shader fromContent(GLenum type, const std::string& content){return Shader(type, content, true);};
};
class Program: public GLResource{
public:
   Program(const std::string& vertexShaderStr, const std::string& fragmentShaderStr, bool isContent);
   inline static Program fromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
      {// 通过优化，如果使用该函数初始化，那么只会执行该构造函数，其他的构造函数都不会执行
         return Program(vertexShaderPath, fragmentShaderPath, false);};
   inline static Program fromContent(const std::string& vertexShaderContent, const std::string& fragmentShaderContent)
      {return Program(vertexShaderContent, fragmentShaderContent, true);};
};
class Texture: public GLResource{
public:
   inline Texture(const char* filepath): Texture(filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr) {}
   Texture(const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor);
};

Texture createTexture(GLint unitId, const char* filepath);

class Context{
private:
   GLFWwindow* window;
public:
   Context(int width, int height);
   ~Context();

   // 移动语义不会合成（由于定义了析构函数），但是拷贝语义会合成，这里删除
   Context(const Context&) = delete;
   Context& operator=(const Context&) = delete;

   inline GLFWwindow* getWindow(){return window;}
};
class Plane{
private:
   GLuint vao;
   Buffer vbo;
   Buffer ebo;
   Texture texture1;
   Texture texture2;
   Buffer createVBO();
   Buffer createEBO();
public:
   Plane();
   ~Plane();

   Plane(const Plane&) = delete;
   Plane& operator=(const Plane&) = delete;
};
class Cube{
private:
   GLuint vao;
   Buffer vbo;
   Texture texture1;
   Texture texture2;
   Buffer createVBO();
public:
   Cube();
   ~Cube();

   Cube(const Cube&) = delete;
   Cube& operator=(const Cube&) = delete;
};

std::optional<std::string> getGlError();
std::optional<std::string> getGlfwError();
void checkGlError_(const char *file, int line);
// __FILE__和__LINE__宏是预定义的，用于表示当前的文件名称和行数
#define checkGlError() checkGlError_(__FILE__, __LINE__) 
void checkGlfwError_(const char *file, int line);
#define checkGlfwError() checkGlfwError_(__FILE__, __LINE__) 
#endif

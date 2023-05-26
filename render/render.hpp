#ifndef RENDER_HPP
#define RENDER_HPP

#include <gl.h>
#include <glfw3.h>
#include <optional>
#include <string>
#include <fmt/format.h>

// buffer、shader、program可以有移动构造函数
// 所有资源都没有赋值操作和拷贝操作


// 用于管理一个opengl的buffer对象的资源
class Buffer{
private:
   GLuint bufferId;
public:
   Buffer(GLenum bufferType, const void* data, GLsizeiptr size, GLenum usage);
   ~Buffer();
   inline Buffer(Buffer&& buffer){this->bufferId = buffer.bufferId; buffer.bufferId = 0;}
   Buffer(const Buffer& buffer) = delete;
   Buffer& operator=(const Buffer& buffer) = delete;

   inline GLuint getId(){return bufferId;}
};
class Shader{
private:
   GLuint shaderId;
public:
   Shader(GLenum type, const std::string& str, bool isContent);
   inline static Shader fromFile(GLenum type, const std::string& path){return Shader(type, path, false);};
   inline static Shader fromContent(GLenum type, const std::string& content){return Shader(type, content, true);};
   ~Shader();
   
   inline Shader(Shader&& shader){this->shaderId = shader.shaderId; shader.shaderId = 0;}
   Shader(const Shader& shader) = delete;
   Shader& operator=(const Shader& shader) = delete;

   inline GLuint getId(){return shaderId;}
};
class Program{
private:
   GLuint programId;
public:
   Program() = default;
   Program(const std::string& vertexShaderStr, const std::string& fragmentShaderStr, bool isContent);
   inline static Program fromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
      {// 通过优化，如果使用该函数初始化，那么只会执行该构造函数，其他的构造函数都不会执行
         return Program(vertexShaderPath, fragmentShaderPath, false);};
   inline static Program fromContent(const std::string& vertexShaderContent, const std::string& fragmentShaderContent)
      {return Program(vertexShaderContent, fragmentShaderContent, true);};
   ~Program();

   inline Program(Program&& program){this->programId = program.programId; program.programId = 0;}
   Program(const Program& program) = delete;
   Program& operator=(const Program& program) = delete;

   inline GLuint getId(){return programId;}
};
class Texture{
private:
   GLuint textureId;
public:
   inline Texture(const char* filepath): Texture(filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr) {}
   Texture(const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor);
   ~Texture();
   
   inline Texture(Texture&& texture){this->textureId = texture.textureId; texture.textureId = 0;}
   Texture(const Texture& texture) = delete;
   Texture& operator=(const Texture& texture) = delete;
   
   inline GLuint getId(){return textureId;}
};
Texture createTexture(GLint unitId, const char* filepath);

class Context{
private:
   GLFWwindow* window;
public:
   Context(int width, int height);
   ~Context();

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

std::optional<std::string> getGlError();
std::optional<std::string> getGlfwError();
void checkGlError_(const char *file, int line);
// __FILE__和__LINE__宏是预定义的，用于表示当前的文件名称和行数
#define checkGlError() checkGlError_(__FILE__, __LINE__) 
void checkGlfwError_(const char *file, int line);
#define checkGlfwError() checkGlfwError_(__FILE__, __LINE__) 
#endif

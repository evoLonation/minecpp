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
#include <stb_image.h>
#include "../exception.hpp"
#include <vector>

namespace render_template{

enum ResourceType{
   BUFFER, VERTEXARRAY, TEXTURE, PROGRAM, SHADER
};

template<ResourceType type>
class GLResource{
private:
   //待特化
   void deleteResource(GLuint id);
   GLuint createResource();
   GLuint createResource(GLenum subType);
protected:
   GLuint id;
   GLResource():id(createResource()){};
   GLResource(GLenum subType):id(createResource(subType)){};
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
GLuint GLResource<SHADER>::createResource(GLenum subType) {
   return glCreateShader(subType);
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
Buffer<bufferType>::Buffer(const void* data, GLsizeiptr size, GLenum usage)
{
   glBindBuffer(bufferType, this->id);
   glBufferData(bufferType, size, data, usage);
   // 如果出错，由于GLResource是基类已经构造完毕
   checkGLError();
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
      checkGLError();
   }
   void bindElemenBuffer(const ElementBuffer& buffer){
      glBindVertexArray(this->id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.getId());
      checkGLError();
   }
};

template<GLenum shaderType>
class Shader: public GLResource<SHADER>{
public:
   Shader(const std::string& str, bool isContent);
   static Shader fromFile(const std::string& path){return Shader(path, false);};
   static Shader fromContent(const std::string& content){return Shader(content, true);};
};
using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

template<GLenum shaderType>
Shader<shaderType>::Shader(const std::string& str, bool isContent):
GLResource(shaderType)
{
   GLuint shader = this->id;
   std::string content;
   if(!isContent){
      // 读取着色器文件
      std::ifstream shaderFile = std::ifstream(str);
      if (!shaderFile.is_open())
      {
         throwError(fmt::format("Open shader file {} failed!", str));
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

   checkGLError();      

   // 判断编译是否错误
   GLint status;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
   if (status == GL_FALSE)
   {
      GLint logLength;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

      GLchar logInfo[logLength + 1];
      glGetShaderInfoLog(shader, logLength, nullptr, logInfo);
      
      throwError(fmt::format("compile shader failure: {}", (char*)logInfo));
   }
}

class Program: public GLResource<PROGRAM>{
public:
   // todo 两个参数都可接受着色器的左右值（使用模板？）
   Program(VertexShader&& vertexShader, FragmentShader&& fragmentShader);
   Program(const VertexShader& vertexShader, const FragmentShader& fragmentShader);
};
inline Program::Program(VertexShader&& vertexShader, FragmentShader&& fragmentShader)
:Program(vertexShader, fragmentShader) {}

inline Program::Program(const VertexShader& vertexShader, const FragmentShader& fragmentShader) {
   GLuint program = this->id;

   glAttachShader(program, vertexShader.getId()); 
   glAttachShader(program, fragmentShader.getId());

   glLinkProgram(program);

   checkGLError();

   // 判断编译是否错误
   GLint status;
   glGetProgramiv(program, GL_LINK_STATUS, &status);
   if (status == GL_FALSE)
   {
      GLint logLength;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

      GLchar logInfo[logLength + 1];
      glGetProgramInfoLog(program, logLength, nullptr, logInfo);

      throwError(fmt::format("link program failure: {}", (char*)logInfo));
   }
   
}
template<GLenum textureType>
class Texture: public GLResource<TEXTURE>{
protected:
   Texture() = default;
public:
   void bindUnit(GLint unit){
      glActiveTexture(GL_TEXTURE0 + unit);
      glBindTexture(textureType, this->id);
      checkGLError();
   }
};

class Texture2D: public Texture<GL_TEXTURE_2D>{
public:
   Texture2D(const char* filepath, GLint unit = 0): Texture2D(unit, filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr) {}
   Texture2D(GLint unit, const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor);
};

Texture2D::Texture2D(GLint unit, const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor){
   GLuint texture = this->id;
   
   bindUnit(unit);

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
      throwError(fmt::format("load image from {} failed", filepath));
   }
   fmt::print("the channel number of image {} : {}\n", filepath, nrChannels);
   if(nrChannels == 3){
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   }else if(nrChannels == 4){
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
   }else{
      stbi_image_free(data);
      throwError(fmt::format("the channel of image is not 3 nor 4, is {}", nrChannels));
   }

   stbi_image_free(data);
   
   if(   magFilterType == GL_LINEAR_MIPMAP_LINEAR ||
         magFilterType == GL_LINEAR_MIPMAP_NEAREST ||
         magFilterType == GL_NEAREST_MIPMAP_LINEAR ||
         magFilterType == GL_NEAREST_MIPMAP_NEAREST) {
      glGenerateMipmap(GL_TEXTURE_2D);
   }

   checkGLError();
}

class Cube{
private:
   VertexBuffer vbo;
   VertexArray vao;
   Texture2D texture1;
   Texture2D texture2;
   VertexBuffer createVBO();
   VertexArray createVAO();
public:
   Cube(): 
   vbo(createVBO()),
   vao(createVAO()), 
   texture1(Texture2D("../image/container.jpg")),
   texture2(Texture2D("../image/awesome-face.png", 1)){};
   ~Cube() = default;

   VertexArray& getVertexArray(){return vao;}

   Cube(const Cube&) = delete;
   Cube& operator=(const Cube&) = delete;
};

inline VertexBuffer Cube::createVBO(){
   GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
   };
   return VertexBuffer(vertices, sizeof(vertices), GL_STATIC_DRAW);
}

inline VertexArray Cube::createVAO(){
   VertexArray vao;
   vao.addAttribute(this->vbo, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(0));
   vao.addAttribute(this->vbo, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
   return vao;
}

class KeyHandler{
private:
   std::function<void(GLFWwindow*)> handler;
   int key;
   int action;
   int mods;
public:
   void handle(GLFWwindow* window, int key, int action, int mods){
      if(this->action == 0){
         if(this->key == key && (action == GLFW_PRESS || action == GLFW_REPEAT)){
            handler(window);
         }
      }else if(this->key == key && this->action == action && this->mods & mods){
         handler(window);
      }
   }
   KeyHandler(int key, int action, int mods, std::function<void(GLFWwindow*)>&& handler):
   handler(handler),key(key),action(action),mods(mods){};
   KeyHandler(int key, int action, std::function<void(GLFWwindow*)>&& handler):
   handler(handler),key(key),action(action), mods(0){};
   KeyHandler(int key, std::function<void(GLFWwindow*)>&& handler):
   handler(handler),key(key),action(0), mods(0){};
};

class Context{
private:
   GLFWwindow* window;
   static std::vector<KeyHandler> handlers;
   static std::vector<std::function<void(GLFWwindow*,int,int)>> sizeHandlers;
public:

   Context(int width, int height);
   ~Context();

   // 移动语义不会合成（由于定义了析构函数），但是拷贝语义会合成，这里删除
   Context(const Context&) = delete;
   Context& operator=(const Context&) = delete;

   GLFWwindow* getWindow(){return window;}

   void addKeyHandler(int key, int action, int mods, std::function<void(GLFWwindow*)>&& handler){
      handlers.emplace_back(key, action, mods, std::move(handler));
   }
   void addKeyHandler(int key, int action, std::function<void(GLFWwindow*)>&& handler){
      handlers.emplace_back(key, action, std::move(handler));
   }
   void addKeyHandler(int key, std::function<void(GLFWwindow*)>&& handler){
      handlers.emplace_back(key, std::move(handler));
   }
   void addSizeChangeHandler(std::function<void(GLFWwindow*,int,int)>&& handler){
      sizeHandlers.push_back(std::move(handler));
   }

   void startLoop(std::function<void(void)> loopHandler);
};
inline std::vector<KeyHandler> Context::handlers;
inline std::vector<std::function<void(GLFWwindow*,int,int)>> Context::sizeHandlers;


inline Context::Context(int width, int height){
   if(glfwInit() != GLFW_TRUE){
      throwError("glfw init failed");
   }

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   GLFWwindow *window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
   if (window == nullptr){
      glfwTerminate();
      throwError("Failed to create GLFW window");
   }
   this->window = window;

   // 将创建的窗口设置为当前opengl上下文
   glfwMakeContextCurrent(window);
   
   // 借助 glad加载opengl的函数
   int version = gladLoadGL(glfwGetProcAddress);
   if (version == 0){
      glfwTerminate();
      throwError("Failed to initialize GLAD");
   }

   fmt::println("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

   glViewport(0, 0, width, height);

   glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height){
      for(auto handler: sizeHandlers){
         handler(window, width, height);
      }
   });
   


   glfwSetKeyCallback(getWindow(), [](GLFWwindow *window, int key, int scancode, int action, int mods){
      for(auto handler: handlers){
         handler.handle(window, key, action, mods);
      }
   });

   glEnable(GL_DEPTH_TEST);
   
   checkGLFWError([](){
      glfwTerminate();
   });

}
inline Context::~Context(){
   glfwTerminate();
}
inline void Context::startLoop(std::function<void(void)> loopHandler){
   // 当没有用户执行了窗口的退出操作后
   while (!glfwWindowShouldClose(getWindow()))
   {
      // 设置清除缓冲区的颜色并清除缓冲区
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      // 同时清除颜色缓冲区和深度缓冲区
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      loopHandler();

      glfwSwapBuffers(getWindow());
      glfwPollEvents();
      checkGLError();
   }
}



};


#endif
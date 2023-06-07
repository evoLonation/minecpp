#ifndef _MINECPP_RESOURCE_H_
#define _MINECPP_RESOURCE_H_

#include <gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fmt/format.h>
#include <functional>
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <tuple>
#include "exception.hpp"
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>




namespace render_template2{


// opengl中的一些具有GLuind 类型id的对象
enum ResourceType{
   BUFFER, VERTEXARRAY, TEXTURE, PROGRAM, SHADER
};

// opengl中的一些具有GLuind 类型id的对象的模板生成方法
// 一些公共特征：
// GLuint 作为id 的对象
// 只能构造和移动，无法拷贝
template<ResourceType type, GLenum subType = 0>
class GLResource{
private:
   void deleteResource(GLuint id);
   GLuint createResource();
protected:
   GLuint id;
   GLResource():id(createResource()){};
public:

   GLResource(GLResource&& resource) noexcept :id(resource.id){resource.id = 0;}
   
   GLResource(const GLResource& resource) = delete;
   GLResource& operator=(const GLResource& resource) = delete;
   GLResource& operator=(GLResource&& resource) = delete;
   ~GLResource(){if(this->id != 0){deleteResource(this->id);}}
   
   GLuint getId() const noexcept{return id;}
};

template<ResourceType type, GLenum subType>
GLuint GLResource<type, subType>::createResource() {
   // todo 添加静态类型断言
   if constexpr(type == BUFFER){
      GLuint id;
      glGenBuffers(1, &id);
      return id;
   }else if constexpr(type == VERTEXARRAY){
      GLuint id;
      glGenVertexArrays(1, &id);
      return id;
   }else if constexpr(type == TEXTURE){
      GLuint id;
      glGenTextures(1, &id);
      return id;
   }else if constexpr(type == PROGRAM){
      return glCreateProgram();
   }else if constexpr(type == SHADER){
      return glCreateShader(subType);
   }else {
      throwError("unknown resource type to create ");
   }
}

template<ResourceType type, GLenum subType>
void GLResource<type, subType>::deleteResource(GLuint id) {
   // todo 添加静态类型断言
   if constexpr(type == BUFFER){
      glDeleteBuffers(1, &id);
   }else if constexpr(type == VERTEXARRAY){
      glDeleteVertexArrays(1, &id);
   }else if constexpr(type == TEXTURE){
      glDeleteTextures(1, &id);
   }else if constexpr(type == PROGRAM){
      glDeleteProgram(id);
   }else if constexpr(type == SHADER){
      glDeleteShader(id);
   }else{
      throwError("unknown resource type to delete");
   }
}

// opengl中的一些 context target 绑定函数
template<ResourceType type, GLenum subType>
void bind_(GLuint resourceId) noexcept {
   if constexpr(type == BUFFER){
      glBindBuffer(subType, resourceId);
   }else if constexpr(type == VERTEXARRAY){
      glBindVertexArray(resourceId);
   }else if constexpr(type == TEXTURE){
      glBindTexture(subType, resourceId);
   }else if constexpr(type == PROGRAM){
      glUseProgram(resourceId);
   }else{
      throwError("unknown resource type to bind");
   }
}

// 对于某个 context target，记录当前的目标，如果等于要绑定的目标，就不用重新执行绑定函数
// 如果实际执行了一次绑定，则返回 true
template<ResourceType type, GLenum subType=0>
bool bindContext(GLuint resourceId, bool must = false) noexcept {
   static GLuint contextTarget = 0;
   if(!must && resourceId == contextTarget){
      return false;
   }else{
      bind_<type, subType>(resourceId);
      contextTarget = resourceId;
      return true;
   }
}

template<ResourceType type, GLenum subType=0>
bool bindContext(const GLResource<type, subType>& resource) noexcept {
   return bindContext<type, subType>(resource.getId());
}

template<ResourceType type, GLenum subType=0>
bool mustBindContext(const GLResource<type, subType>& resource) noexcept {
   return bindContext<type, subType>(resource.getId(), true);
}


template<GLenum bufferType>
class Buffer: public GLResource<BUFFER, bufferType>{
public:
   Buffer(const void* data, GLsizeiptr size, GLenum usage){
      bindContext(*this);
      glBufferData(bufferType, size, data, usage);
      checkGLError();
   }
};

using VertexBuffer = Buffer<GL_ARRAY_BUFFER>;
using ElementBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER>;

class VertexArray: public GLResource<VERTEXARRAY>{
public:
   void addAttribute(
      const VertexBuffer& buffer,
      GLuint index, 
      GLint size, 
      GLenum type, 
      GLboolean normalized, 
      GLsizei stride, 
      const void * pointer){
         // bindContext(*this);
         // // 对于 array buffer 上下文目标是全局的
         // bindContext(buffer);
         glBindVertexArray(this->id);
         glBindBuffer(GL_ARRAY_BUFFER, buffer.getId());
         glVertexAttribPointer(index, size, type, normalized, stride, pointer);
         glEnableVertexAttribArray(index);
         checkGLError();
   }

   void bindElemenBuffer(const ElementBuffer& buffer){
      // element buffer 上下文目标是局部的，与vertex array 绑定
      if(bindContext(*this)){
         mustBindContext(buffer);
      }else{
         bindContext(buffer);
      }
      checkGLError();
   }
};

template<GLenum shaderType>
class Shader: public GLResource<SHADER, shaderType>{
public:
   Shader(const std::string& str, bool isContent);
   static Shader fromFile(const std::string& path){return Shader(path, false);};
   static Shader fromContent(const std::string& content){return Shader(content, true);};
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
private:
   // 存储 program 中的已知的 uniform 及其 location
   std::map<std::string, GLint> uniforms; 
public:
   // 使用万能引用
   template<typename VS, typename FS>
   Program(VS&& vertexShader, FS&& fragmentShader);

   template<typename T>
   void setUniform(const std::string name, const T& value){
      bindContext(*this);
      // 懒汉获取uniform的location
      if(!uniforms.contains(name)){
         GLint location = glGetUniformLocation(id, name.c_str());
         if(location == -1){
            throwError(fmt::format("the uniform {} does not exist or illegal in the program id {}\n\
            notice: opengl may by optimise some useless (direct or indirect) uniform, so check if {} is actually used", name, getId(), name));
         }
         uniforms[name] = location;
      }
      setUniformFunc(uniforms[name], name, value);
   }
private:
   template<typename T>
   void setUniformFunc(GLint location, const std::string name, const T& value);
};

template<typename VS, typename FS>
Program::Program(VS&& vertexShader, FS&& fragmentShader) {
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

template<>
void Program::setUniformFunc(GLint location, const std::string name, const glm::mat4& value){
   glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
template<>
void Program::setUniformFunc(GLint location, const std::string name, const glm::vec3& value){
   glUniform3fv(location, 1, glm::value_ptr(value));
}
template<>
void Program::setUniformFunc(GLint location, const std::string name, const glm::mat3& value){
   glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
template<>
void Program::setUniformFunc(GLint location, const std::string name, const GLint& value){
   glUniform1i(location, value);
}
template<>
void Program::setUniformFunc(GLint location, const std::string name, const GLfloat& value){
   glUniform1f(location, value);
}

template<GLenum textureType>
class Texture: public GLResource<TEXTURE, textureType>{};

class TextureUnit{
private:
   static GLuint context[GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS];
   static void bindUnit(GLint unit) noexcept{
      static GLint activeUnit = 0;
      if(unit != activeUnit){
         glActiveTexture(GL_TEXTURE0 + unit);
         activeUnit = unit;
      }
   }
public:
   template<GLenum subType>
   static void bind(GLint unit, Texture<subType>& texture) {
      if(context[unit] != texture.getId()){
         bindUnit(unit);
         bindContext(texture);
         context[unit] = texture.getId();
      }
   }
};

inline GLuint TextureUnit::context[GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS];



class Texture2D: public Texture<GL_TEXTURE_2D>{
public:
   Texture2D(const char* filepath, GLint unit = 0): Texture2D(unit, filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr) {}
   Texture2D(GLint unit, const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, glm::vec4* borderColor);
};

Texture2D::Texture2D(GLint unit, const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, glm::vec4* borderColor){
   GLuint texture = this->id;

   TextureUnit::bind(unit, *this);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizonType);	
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalType);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterType);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterType);

   if(horizonType == GL_CLAMP_TO_BORDER || verticalType == GL_CLAMP_TO_BORDER){
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(*borderColor));
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


// 按照先后顺序保存一次链式调用中被改变的所有对象
inline std::vector<std::function<void(void)>*> chainCall;
inline std::set<std::function<void(void)>*> chainCallSet;

// 用于存储数据是否“脏”
// 每个observer可以产生多个关联的dealer
// 保证同一个dealer在一个observer的数据修改后的多次调用中，只有第一次调用真正执行了逻辑
template<typename T>
class DirtyObservable{
private:
   T mValue;
   std::vector<std::function<void(void)>> observers;
   void publish() noexcept{
      
      for(auto& observer : observers){
         // 如果调用链中包含此对象，说明发生了循环调用，直接返回
         if(chainCallSet.contains(&observer)){
            continue;
         }
         // 在调用链中添加自己
         chainCall.push_back(&observer);
         chainCallSet.insert(&observer);
         // 其中也可能发生调用
         observer();
         // 在调用链中删除自己
         chainCall.pop_back();
         chainCallSet.erase(&observer);
      }
   }
public:
   DirtyObservable(T&& t)noexcept:mValue(std::move(t)){}
   
   DirtyObservable& operator=(const T& t)noexcept{
      
      mValue = t;
      publish();
      return *this;
   }
   DirtyObservable& operator=(T&& t)noexcept{
      mValue = std::move(t);
      publish();
      return *this;
   }
   const T& value()noexcept{ return mValue; }
   
   void addObserver(const std::function<void(void)>& observer)noexcept{
      observers.push_back(observer);
   }

};


class DrawUnit{
private:
   VertexArray& vao;
   Program& program;
   std::map<int, Texture2D*> textures;
   std::vector<std::function<void(void)>> uniformSetters;
public:
   DrawUnit(VertexArray& vao, Program& program):vao(vao), program(program){}

   // addUniform函数保证每次draw时program绑定的uniform一定是当前这个值
   // 要知道 program 的哪些 uniform 在其他drawunit中被修改了
   // 遍历每个 uniform ，如果被修改了则肯定执行；如果没有被修改则
   template<typename T>
   void addUniform(const std::string& name, const T& value){
      uniformSetters.push_back([this, &value, name](){
         this->program.setUniform(name, value);
      });
   }
   void addTexture(Texture2D& texture, GLint unit=0){
      textures.insert({unit, &texture});
   }
   void draw(){
      bindContext(vao);
      bindContext(program);
      for(auto& setter: uniformSetters){
         setter();
      }
      for(auto& texture: textures){
         TextureUnit::bind(texture.first, *texture.second);
      }
   }
};

template<typename T>
class Singleton{
protected:
   Singleton() = default;
public:
   Singleton& operator=(Singleton const &) = delete;
   Singleton(Singleton const &) = delete;
   static T& getInstance(){
      static T instance;
      return instance;
   }

};
class Context: public Singleton<Context>{
   friend Singleton<Context>;
private:
   Context(){
      if(glfwInit() != GLFW_TRUE){
         throwError("glfw init failed");
      }
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   }
   GLFWwindow* window;
   DirtyObservable<int>* width;
   DirtyObservable<int>* height;
   ~Context(){glfwTerminate();}
   
public:
   GLFWwindow* getWindow(){return window;}
   void createWindow(int width, int height);
};

void Context::createWindow(int width, int height)
{

   this->width = new DirtyObservable(std::move(width));
   this->height = new DirtyObservable(std::move(height));
   this->window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
   if (window == nullptr){
      glfwTerminate();
      throwError("Failed to create GLFW window");
   }

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

   glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int newWidth, int newHeight){
      glViewport(0, 0, newWidth, newHeight);
      auto& ctx = Context::getInstance();
      *ctx.width = newWidth;
      *ctx.height = newHeight;
   });
   
   glEnable(GL_DEPTH_TEST);
   
   checkGLFWError([](){
      glfwTerminate();
   });
}

class InputProcessor: public Singleton<InputProcessor>{
friend class Singleton<InputProcessor>;
private:
   std::map<int, std::chrono::_V2::system_clock::time_point> pressedKeys;
   std::map<int, std::vector<std::function<void()>>> keyDownHandlers;
   std::map<int, std::vector<std::function<void()>>> keyUpHandlers;
   std::map<int, std::vector<std::function<void(int)>>> keyReleaseHandlers;
   std::map<int, std::vector<std::function<void(int)>>> keyHoldHandlers;

   InputProcessor() {
      glfwSetKeyCallback(Context::getInstance().getWindow(), [](GLFWwindow *window, int key, int scancode, int action, int mods){
         auto& processor = InputProcessor::getInstance();
         if(action == GLFW_PRESS){
            if(processor.keyDownHandlers.contains(key)){
               for(const auto& handler : processor.keyDownHandlers[key]){
                  handler();
               }
            }
            auto startTime = std::chrono::high_resolution_clock::now();
            processor.pressedKeys.insert({key, startTime});
         }else if(action == GLFW_RELEASE){
            if(processor.keyUpHandlers.contains(key)){
               for(const auto& handler : processor.keyUpHandlers[key]){
                  handler();
               }
            }
            if(processor.keyReleaseHandlers.contains(key)){
               auto startTime = processor.pressedKeys[key];
               auto endTime = std::chrono::high_resolution_clock::now();
               auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
               for(const auto& handler : processor.keyReleaseHandlers[key]){
                  handler(duration.count());
               }
            }
            processor.pressedKeys.erase(key);
         }

      });
   };
public:
   void addKeyDownHandler(int key, const std::function<void()>& handler){
      keyDownHandlers[key].push_back(handler);
   }
   void addKeyUpHandler(int key, const std::function<void()>& handler){
      keyUpHandlers[key].push_back(handler);
   }
   void addKeyHoldHandler(int key, const std::function<void(int milli)>& handler){
      keyHoldHandlers[key].push_back(handler);
   }
   void addKeyReleaseHandler(int key, const std::function<void(int holdMilli)>& handler){
      keyReleaseHandlers[key].push_back(handler);
   }
   void processInput(){
      static auto _lastTime = std::chrono::high_resolution_clock::now();
      auto _nowTime = std::chrono::high_resolution_clock::now();
      auto _duration = std::chrono::duration_cast<std::chrono::milliseconds>(_nowTime - _lastTime);
      if(_duration.count() < 16.7){
         return;
      }else{
         _lastTime = std::chrono::high_resolution_clock::now();
      }

      bool hasAny = false;
      for(const auto& pair: pressedKeys){
         auto _key = pair.first;
         std::chrono::_V2::system_clock::time_point endTime;
         if(keyHoldHandlers.contains(_key)){
            if(!hasAny){
               endTime = std::chrono::high_resolution_clock::now();
               hasAny = true;
            }
            auto startTime = pair.second;
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            for(const auto& handler : keyHoldHandlers[_key]){
               handler(duration.count());
            }
         }
      }
   }
};

};
#endif // _MINECPP_RESOURCE_H_
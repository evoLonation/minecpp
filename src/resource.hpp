#ifndef _MINECPP_RESOURCE_H_
#define _MINECPP_RESOURCE_H_

// gl.h的include必须在glfw之前
#include <cinttypes>
#include <concepts>
#include <corecrt.h>
#include <cstddef>
#include <gl.h>
#include <GLFW/glfw3.h>
#include <initializer_list>
#include <optional>
#include <string>
#include <fmt/format.h>
#include <functional>
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <tuple>
#include "assimp/light.h"
#include "assimp/material.h"
#include "exception.hpp"
#include <variant>
#include <vector>
#include <map>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "fmt/core.h"
#include "glm/fwd.hpp"
#include "tool.hpp"
#include "gui.hpp"
#include <type_traits>

namespace minecpp{

/**********************/

// 以 GLResource 为基类的一系列类型的职责是：
// 1. 维护 opengl 中资源的生命周期（等价于对象及被移动到的另一个对象，以此类推，的生命周期之和）
// 2. 管理 opengl 的资源
// 这些类型之间没有聚合关系，只有借助资源进行设置的关系；如果资源A作为参数被资源B使用，请保证资源A的生命周期比资源B长 
// 上面的规则的例外是Program和Shader； Shader链接为Program后可以被销毁

/**********************/

/*****************************************************/
/*****************************************************/
/***************** RESOURCE CONTEXT ******************/
/*****************************************************/
/*****************************************************/

enum class ResourceType{
   BUFFER, VERTEX_ARRAY, TEXTURE, PROGRAM, SHADER
};

// subType：一些资源的子类型，如buffer还有 vbo, ebo
template<ResourceType type, GLenum subType = 0>
class Resource{
private:
   GLuint id;

   GLuint createResource(){
      if constexpr(type == ResourceType::BUFFER){
         GLuint id;
         glGenBuffers(1, &id);
         return id;
      }else if constexpr(type == ResourceType::VERTEX_ARRAY){
         GLuint id;
         glGenVertexArrays(1, &id);
         return id;
      }else if constexpr(type == ResourceType::TEXTURE){
         GLuint id;
         glGenTextures(1, &id);
         return id;
      }else if constexpr(type == ResourceType::PROGRAM){
         return glCreateProgram();
      }else if constexpr(type == ResourceType::SHADER){
         return glCreateShader(subType);
      }else {
         // 如果直接使用static_assert，则不管编译后还有没有这个block，还是会报错
         // 如果将static_assert包装在lambda中调用，则只有进来这个block后才会进行实例化，从而才会报错
         []<bool flag = false>(){static_assert(flag);}();
      }
   }

   void deleteResource(GLuint id){
      if constexpr(type == ResourceType::BUFFER){
         glDeleteBuffers(1, &id);
      }else if constexpr(type == ResourceType::VERTEX_ARRAY){
         glDeleteVertexArrays(1, &id);
      }else if constexpr(type == ResourceType::TEXTURE){
         glDeleteTextures(1, &id);
      }else if constexpr(type == ResourceType::PROGRAM){
         glDeleteProgram(id);
      }else if constexpr(type == ResourceType::SHADER){
         glDeleteShader(id);
      }else{
         []<bool flag = false>(){static_assert(flag);}();
      }
   }

   void moved(){
      id = 0;
   }

   void mayDelete(){
      if(this->id != 0){
         deleteResource(this->id);
      }
   }

public:
   Resource():id(createResource()){};

   Resource(Resource&& resource) :id(resource.id){
      resource.moved();
   }
   Resource& operator=(Resource&& resource) {
      mayDelete();
      id = resource.id;
      resource.moved();
      return *this;
   }
   
   Resource(const Resource& resource) = delete;
   Resource& operator=(const Resource& resource) = delete;

   ~Resource(){
      mayDelete();
   }

   GLuint getId() const {
      return id;
   }
};

template<GLenum bufferType>
using BufferRsc = Resource<ResourceType::BUFFER, bufferType>;
using VertexBufferRsc = BufferRsc<GL_ARRAY_BUFFER>;
using ElementBufferRsc = BufferRsc<GL_ELEMENT_ARRAY_BUFFER>;

using VertexArrayRsc = Resource<ResourceType::VERTEX_ARRAY>;

template<GLenum shaderType>
using ShaderRsc = Resource<ResourceType::SHADER, shaderType>;
using VertexShaderRsc = ShaderRsc<GL_VERTEX_SHADER>;
using FragmentShaderRsc = ShaderRsc<GL_FRAGMENT_SHADER>;

using ProgramRsc = Resource<ResourceType::PROGRAM>;

template<GLenum textureType>
using TextureRsc = Resource<ResourceType::TEXTURE, textureType>;
using Texture2DRsc = TextureRsc<GL_TEXTURE_2D>;

enum class ContextType{
   BUFFER, VERTEX_ARRAY, TEXTURE, PROGRAM
};

template<ContextType type, GLenum subType = 0>
class ResourceContext{
private:
   GLuint contextTarget = 0;

   void bind(GLuint resourceId)  {
      if constexpr(type == ContextType::BUFFER){
         glBindBuffer(subType, resourceId);
      }else if constexpr(type == ContextType::VERTEX_ARRAY){
         glBindVertexArray(resourceId);
      }else if constexpr(type == ContextType::TEXTURE){
         glBindTexture(subType, resourceId);
      }else if constexpr(type == ContextType::PROGRAM){
         glUseProgram(resourceId);
      }else{
         []<bool flag = false>(){static_assert(flag);}();
      }
      checkGLError();
   }

   static constexpr ResourceType ctx2rse(ContextType ctx){
      switch(ctx){
      case ContextType::BUFFER: return ResourceType::BUFFER;
      case ContextType::VERTEX_ARRAY: return ResourceType::VERTEX_ARRAY;
      case ContextType::TEXTURE: return ResourceType::TEXTURE;
      case ContextType::PROGRAM: return ResourceType::PROGRAM;
      default: throw "unsupported";
      };
   }

protected:
   ResourceContext() = default;

   void bindContext(const Resource<ctx2rse(type), subType>& resource){
      bind(resource.getId());
   }

   void unBind(){
      bind(0);
   }
};

template<GLenum bufferType>
using BufferContext = ResourceContext<ContextType::BUFFER, bufferType>;

class VertexBufferContext: private BufferContext<GL_ARRAY_BUFFER>, public ProactiveSingleton<VertexBufferContext>{
public:
   VertexBufferContext() = default;
   using BufferContext<GL_ARRAY_BUFFER>::bindContext;
};

class VertexArrayContext: private ResourceContext<ContextType::VERTEX_ARRAY>, public ProactiveSingleton<VertexArrayContext>{
public:
   VertexArrayContext() = default;
   using ResourceContext<ContextType::VERTEX_ARRAY>::bindContext;
   using ResourceContext<ContextType::VERTEX_ARRAY>::unBind;
};

// ElementBuffer 分别具有全局的和隶属于 VertexArray 的 Context
class GlobalElementBufferContext: private BufferContext<GL_ELEMENT_ARRAY_BUFFER>, public ProactiveSingleton<GlobalElementBufferContext>{
public:
   GlobalElementBufferContext() = default;
   void bindContext(const ElementBufferRsc& ebo){
      VertexArrayContext::getInstance().unBind();
      BufferContext<GL_ELEMENT_ARRAY_BUFFER>::bindContext(ebo);
   }
};

class VAORscWithEBOContext: public VertexArrayRsc, private BufferContext<GL_ELEMENT_ARRAY_BUFFER>{
public:
   VAORscWithEBOContext() = default;
   void bindContext(const ElementBufferRsc& ebo){
      VertexArrayContext::getInstance().bindContext(*this);
      BufferContext<GL_ELEMENT_ARRAY_BUFFER>::bindContext(ebo);
   }
};

class ProgramContext: private ResourceContext<ContextType::PROGRAM>, public ProactiveSingleton<ProgramContext>{
public:
   ProgramContext() = default;
   using ResourceContext<ContextType::PROGRAM>::bindContext;
};

template<GLenum textureType>
using TextureContext = ResourceContext<ContextType::TEXTURE, textureType>;

// 存储某个具体类型纹理在所有纹理单元中的状态
template<GLenum textureType>
class TextureUnitFor: public ProactiveSingleton<TextureUnitFor<textureType>>{
private:
   class MTextureContext: private TextureContext<textureType>{
   public:
      MTextureContext() = default;
      using TextureContext<textureType>::bindContext;
   };
   MTextureContext context[GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS];
   
   GLint activeUnit = 0;
   void activateUnit(GLint unit) {
      if(unit != activeUnit){
         glActiveTexture(GL_TEXTURE0 + unit);
         checkGLError();
         activeUnit = unit;
      }
   }
public:
   void bindUnit(GLint unit, const TextureRsc<textureType>& texture){
      activateUnit(unit);
      context[unit].bindContext(texture);
   }
};

class TextureUnit: public ProactiveSingleton<TextureUnit> {
private:
   TextureUnitFor<GL_TEXTURE_2D> texture2DUnit;

   template<GLenum textureType>
   TextureUnitFor<textureType>& getSpecificContext(){
      if constexpr (textureType == GL_TEXTURE_2D){
         return texture2DUnit;
      }else{
         throw "unsupported";
      }
   }
public:
   template<GLenum textureType>
   void bindUnit(GLint unit, const TextureRsc<textureType>& texture) {
      getSpecificContext<textureType>().bindUnit(unit, texture);
   }
};

/*****************************************************/
/*****************************************************/
/*******************     BUFFER     ******************/
/*****************************************************/
/*****************************************************/

// 创建一个指定类型的buffer并设置其为当前上下文，同时绑定数据
template<GLenum bufferType>
class Buffer: public BufferRsc<bufferType>{
private:
   using BaseRsc = BufferRsc<bufferType>;

   // CRTP 实现静态多态
   void dataSettingContext(){
      if constexpr (std::same_as<BaseRsc, VertexBufferRsc>){
         VertexBufferContext::getInstance().bindContext(*this);
      }else if constexpr (std::same_as<BaseRsc, ElementBufferRsc>) {
         GlobalElementBufferContext::getInstance().bindContext(*this);
      }
   }

   Buffer(const void* data, GLsizeiptr size, GLenum usage){
      dataSettingContext();
      // 设置GL_ARRAY_BUFFER上下文对象的数据
      // GL_STREAM_DRAW：数据只设置一次，GPU最多使用几次
      // GL_STATIC_DRAW：数据只设置一次，使用多次
      // GL_DYNAMIC_DRAW：数据变化很大，使用次数也很多
      glBufferData(bufferType, size, data, usage);
      checkGLError();
   }
protected:
   Buffer(const ContiguousContainer auto& data, GLenum usage): 
      Buffer(dataAddress(data), sizeOfData(data), usage){}
};

class VertexBuffer: public Buffer<GL_ARRAY_BUFFER>{
public:
   VertexBuffer(const ContiguousContainer auto& data, GLenum usage = GL_STATIC_DRAW): Buffer<GL_ARRAY_BUFFER>(data, usage){}
};

class ElementBuffer: public Buffer<GL_ELEMENT_ARRAY_BUFFER>{
private:
   int number;
public:
   ElementBuffer(const ContiguousContainerOf<unsigned int> auto& data): 
      Buffer<GL_ELEMENT_ARRAY_BUFFER>(data, GL_STATIC_DRAW), number(sizeOf(data)){}
   
   int getNumber() const { return number; }
};

/*****************************************************/
/*****************************************************/
/******************  VERTEX ARRAY  *******************/
/*****************************************************/
/*****************************************************/

template<typename Type>
concept FloatBase = 
   std::same_as<Type, float> ||
   std::same_as<Type, glm::vec2> ||
   std::same_as<Type, glm::vec3> ||
   std::same_as<Type, glm::vec4>;

class VertexArray: public VAORscWithEBOContext{
private:
   bool bindEBO = false;
   // 顶点的数量
   int number = 0;

private:
   void attributeContext(const VertexBuffer& vbo) const {
      VertexArrayContext::getInstance().bindContext(*this);
      // 对于 array buffer 上下文目标是全局的, vao 和 vbo 的上下文没有什么直接关系
      VertexBufferContext::getInstance().bindContext(vbo);
   }

   void addAttribute(
      const VertexBuffer& vbo,
      GLuint index, 
      GLint size, 
      GLenum type, 
      GLboolean normalized, 
      GLsizei stride, 
      const void * pointer)
   {
      attributeContext(vbo);
      // 设置顶点属性时，相当于为当前上下文中的 vao 和 vbo 设置了关联
      // index: 对应着色器中的location
      // size: 对应单个属性的元素的个数，比如vec3对应3
      // type: 数据的元素的类型，一般来说和属性的类型保持一致（如果不一致则会进行类型转换），如vec3的基本类型是float，则对应GL_FLOAT
      // normalized: 如果设置为GL_TRUE，那么如果输入的值的类型是整形且对应着色器属性的类型是浮点型，就会映射到[-1, 1]（无符号则为[0, 1]）
      // stride: 步长，多少字节拿一次数据
      // offset：从所给数组的那里开始拿数据
      // 注：这里如果和着色器的类型没有完全对应其实也有可能正常运行
      glVertexAttribPointer(index, size, type, normalized, stride, pointer);
      // 启用当前vao中index对应的顶点属性
      glEnableVertexAttribArray(index);
      checkGLError();
   }

   template<typename Type>
   constexpr std::pair<GLint, GLenum> getTypeInfo(){
      if(std::is_same_v<Type, glm::vec3>){
         return {3, GL_FLOAT};
      }else if(std::is_same_v<Type, glm::vec2>){
         return {2, GL_FLOAT};
      }else if(std::is_same_v<Type, glm::vec4>){
         return {4, GL_FLOAT};
      }
      throw "unsupported";
   }

public:
   template<FloatBase Type>
   void addAttribute(const VertexBuffer& buffer, unsigned int index, std::size_t stride, std::size_t offset){
      auto [size, type] = getTypeInfo<Type>();
      addAttribute(buffer, index, size, type, false, stride, reinterpret_cast<const void*>(offset));
   }
   template<FloatBase Type>
   void addAttribute(VertexBuffer&& buffer, unsigned int index, std::size_t stride, std::size_t offset) = delete;

   void bindElementBuffer(const ElementBuffer & ebo){
      // element buffer 上下文目标是局部的，与vertex array 绑定
      bindContext(ebo);
      checkGLError();
      bindEBO = true;
      number = ebo.getNumber();
   }
   void bindElementBuffer(ElementBuffer && ebo) = delete;

   bool isBindEBO() const {return bindEBO;}

   int getNumber() const {
      if(number == 0){
         throwError("not yet set number");
      }
      return number;
   }

   void setNumber(int number){
      this->number = number;
   }
};

/*****************************************************/
/*****************************************************/
/******************     SHADER     *******************/
/*****************************************************/
/*****************************************************/

template<GLenum shaderType, typename Derived>
class Shader: public ShaderRsc<shaderType>{
public:
   Shader(const std::string& str, bool isContent){
      GLuint shader = this->getId();
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
      const char* c_content = content.c_str();
      // 第二个参数是字符串的数量
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
   static Derived fromFile(const std::string& path){return Derived(path, false);};
   static Derived fromContent(const std::string& content){return Derived(content, true);};
};

class VertexShader:public Shader<GL_VERTEX_SHADER, VertexShader>{
   using Shader<GL_VERTEX_SHADER, VertexShader>::Shader;
};
class FragmentShader:public Shader<GL_FRAGMENT_SHADER, FragmentShader>{
   using Shader<GL_FRAGMENT_SHADER, FragmentShader>::Shader;
};

/*****************************************************/
/*****************************************************/
/******************    PROGRAM     *******************/
/*****************************************************/
/*****************************************************/

using UniformDataPack = TypePack<int, float, glm::vec2, glm::vec3, glm::mat3, glm::mat4>;

template<typename ValueType>
concept UniformType = InsertS<ValueType, UniformDataPack>::type::template apply<AllowList>::value;

// template<typename ValueType>
// concept UniformType = 
//    std::same_as<ValueType, glm::vec3> ||
//    std::same_as<ValueType, glm::vec2> ||
//    std::same_as<ValueType, float>     ||
//    std::same_as<ValueType, glm::mat4> ||
//    std::same_as<ValueType, glm::mat3> ||
//    std::same_as<ValueType, int>;

class Program: public ProgramRsc{
private:
   // 存储 program 中的已知的 uniform 及其 location
   std::map<std::string, GLint> uniforms;
public:
   Program(const VertexShader&  vertexShader, const FragmentShader&  fragmentShader){
      GLuint program = this->getId();

      // 将shader加入program
      glAttachShader(program, vertexShader.getId()); 
      glAttachShader(program, fragmentShader.getId());

      // 链接program
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
private:
   GLint getUniformLocation(const std::string& name) {
      // 懒汉获取uniform的location
      if(!uniforms.contains(name)){
         GLint location = glGetUniformLocation(getId(), name.c_str());
         if(location == -1){
            throwError(fmt::format("the uniform {} does not exist or illegal in the program id {}\n\
            notice: opengl may by optimize some useless (direct or indirect) uniform, so check if {} is actually used", name, getId(), name));
         }
         uniforms[name] = location;
         return location;
      }
      return uniforms[name];
   }

   template<UniformType DataType>
   void setUniformFunc(GLint location, const DataType& value) {
      if constexpr (std::same_as<DataType, glm::mat4>){
         glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
      } else if constexpr (std::same_as<DataType, glm::vec3>){
         glUniform3fv(location, 1, glm::value_ptr(value));
      } else if constexpr (std::same_as<DataType, glm::mat3>){
         glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
      } else if constexpr (std::same_as<DataType, GLint>){
         glUniform1i(location, value);
      } else if constexpr (std::same_as<DataType, GLfloat>){
         glUniform1f(location, value);
      }
   }

public:
   template<UniformType DataType>
   void setUniform(const std::string& name, const DataType& value){
      ProgramContext::getInstance().bindContext(*this);
      setUniformFunc(getUniformLocation(name), value);
      checkGLError();
   }
};

/*****************************************************/
/*****************************************************/
/******************    TEXTURE     *******************/
/*****************************************************/
/*****************************************************/

class Texture2D: public Texture2DRsc{
public:
   // opengl 默认的unit就是GL_TEXTURE0
   Texture2D(const std::string& filepath, GLint unit = 0): Texture2D(unit, filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr) {}
   
   // 在指定的纹理单元中创建2D纹理
   // filepath: 图片文件
   // horizonType, verticalType: 横向、纵向扩展类型
   // 可选 GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
   // minFilterType, magFilterType: 纹理缩小/放大过滤方式
   // 可选GL_[NEAREST|LINEAR]、 GL_[NEAREST|LINEAR]_MIPMAP_[NEAREST|LINEAR]（仅magFilterType）
   // borderColor：当设置为 GL_CLAMP_TO_BORDER 时的颜色，需要大小为4的数组(rgba)
   Texture2D(GLint unit, const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, glm::vec4* borderColor){
      GLuint texture = this->getId();
      

      // 每个 texture unit 都可以同时绑定每个类型的纹理对象各一个；但是一个着色器只允许绑定一个sampler
      TextureUnit::getInstance().bindUnit(unit, *this);

      // 设置纹理的参数
      // GL_TEXTURE_WRAP_S: 横向；GL_TEXTURE_WRAP_T：纵向；
      // 过滤方式：GL_TEXTURE_MIN_FILTER: 纹理缩小（图像离得很远）；GL_TEXTURE_MAG_FILTER: 纹理放大（图像离得很近）；
      // 对于GL_TEXTURE_MAG_FILTER，他只能设置GL_NEAREST和GL_LINEAR两种，因为肯定会使用第一级别的mipmap
      // 如果GL_TEXTURE_MIN_FILTER的过滤方式设置为和MipMap相关的，那么在加载纹理数据后需要使用glGenerateMipmap生成mipmap
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizonType);	
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalType);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterType);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterType);

      // 设置GL_CLAMP_TO_BORDER参数后，需要额外自定义颜色
      if(horizonType == GL_CLAMP_TO_BORDER || verticalType == GL_CLAMP_TO_BORDER){
         glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(*borderColor));
      }
      
      int width, height, nrChannels;
      // opengl的纹理坐标的y轴是从底向上从0到1的，但是stb默认加载是从上向下加载的，因此需要翻转一下y轴
      stbi_set_flip_vertically_on_load(true);
      // 读取图片并返回数据、图片的宽高和通道数
      unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
      if(data == nullptr){
         throwError(fmt::format("load image from {} failed", filepath));
      }
      fmt::print("the channel number of image {} : {}\n", filepath, nrChannels);
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
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      }else{
         stbi_image_free(data);
         throwError(fmt::format("the channel of image is not 3 nor 4, is {}", nrChannels));
      }

      stbi_image_free(data);
      
      // 如果过滤方式需要mipmap，则生成mipmap
      if(magFilterType == GL_LINEAR_MIPMAP_LINEAR ||
         magFilterType == GL_LINEAR_MIPMAP_NEAREST ||
         magFilterType == GL_NEAREST_MIPMAP_LINEAR ||
         magFilterType == GL_NEAREST_MIPMAP_NEAREST) {
            glGenerateMipmap(GL_TEXTURE_2D);
      }

      checkGLError();
   }
};

/*****************************************************/
/*****************************************************/
/******************    CONTEXT     *******************/
/*****************************************************/
/*****************************************************/

class GLFWContext: public ProactiveSingleton<GLFWContext>{
public:
   GLFWContext(int majorVersion, int minorVersion){
      if(glfwInit() != GLFW_TRUE){
         throwError("glfw init failed");
      }

      // 设置opengl版本3.3,类型为core profile
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      checkGLFWError([](){
         glfwTerminate();
      });
   }
   ~GLFWContext(){
      glfwTerminate();
   }
};

class GLWindow{
private:
   GLFWwindow* window;
   std::string title;
public:
   GLWindow(int width, int height, std::string title): 
      title(std::move(title))
   {
      // 创建窗口
      window = glfwCreateWindow(width, height, this->title.c_str(), NULL, NULL);
      if (window == nullptr){
         throwError("Failed to create GLFW window");
      }

      setCurrent();
   }
   ~GLWindow(){
      glfwDestroyWindow(window);
   }
   void setCurrent(){
      // 将创建的窗口设置为当前opengl上下文
      glfwMakeContextCurrent(window);
      
      // 借助 glad加载opengl的函数
      int version = gladLoadGL(glfwGetProcAddress);
      if (version == 0){
         glfwTerminate();
         throwError("Failed to initialize GLAD");
      }

      fmt::println("Loaded OpenGL {}.{} for window {}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version), title);
   }

   // deleted copy semantic
   GLWindow& operator=(const GLWindow&) = delete;
   GLWindow(const GLWindow&) = delete;

   GLFWwindow* getWindow() const { return window; }
};

class Context: public ProactiveSingleton<Context>{
private:
   const int majorVersion;
   const int minorVersion;
   ObservableValue<int> width;
   ObservableValue<int> height;
private:
   GLFWContext glfwCtx;
   GLWindow window;
private:
   VertexBufferContext vboCtx;
   GlobalElementBufferContext eboCtx;
   VertexArrayContext vaoCtx;
   ProgramContext programCtx;
   TextureUnit textureUnit;
private:
   GuiContext guiCtx;

public:
   Context(int width, int height)
   :majorVersion(3), minorVersion(3),
   width(width),height(height),
   glfwCtx(majorVersion, minorVersion),
   window(width, height, "LearnOpengl"),
   guiCtx(window.getWindow(), majorVersion, minorVersion)
   {
      // 设置当窗口尺寸变化时的回调函数
      // 还有很多的回调函数，如处理输入等等；须在创建窗口后、开始渲染前注册回调函数
      glfwSetFramebufferSizeCallback(window.getWindow(), [](GLFWwindow *window, int newWidth, int newHeight){
         auto& ctx = Context::getInstance();
         ctx.height = newHeight;
         ctx.width = newWidth;
      });
      checkGLFWError();
   }

   ~Context() = default;
   
   void startLoop(const std::function<void(void)>& loop){
      while (!glfwWindowShouldClose(window.getWindow())){
         loop();
      }
   }
   GLFWwindow* getWindow() const {return window.getWindow();}
   ObservableValue<int>& getWidth(){return width;}
   ObservableValue<int>& getHeight(){return height;}

   int getMajorVersion() const { return majorVersion; }
   int getMinorVersion() const { return minorVersion; }
};

/*****************************************************/
/*****************************************************/
/********************* DRAW_UNIT **********************/
/*****************************************************/
/*****************************************************/

class DrawUnit: public AutoLoader<DrawUnit>{
private:
   const VertexArray* vao;
   Program* program;

   using TextureData = std::tuple<GLint, std::string, const Texture2D*>;
   std::vector<TextureData> textures;

   // 可以是指向输入参数，也可以指向constUniforms的索引（int）
   using UniformDataPtr = Insert<MapTo<MapTo<UniformDataPack, std::add_const_t>, std::add_pointer_t>, std::size_t>::apply<std::variant>;
   using UniformData = std::pair<std::string, UniformDataPtr>;
   std::vector<UniformData> uniforms;

   using UniformConst = UniformDataPack::apply<std::variant>;
   std::vector<UniformConst> constUniforms;

   GLenum mode;

   bool isEnable;

   RefContainer<DrawUnit>& getRefContainer();

   struct UniformRefVariant{
      using UniformDataRef = MapTo<MapTo<UniformDataPack, std::add_const_t>, std::reference_wrapper>::apply<std::variant>;
      UniformDataRef ref;
      bool constant;
      template<UniformType Type>
      UniformRefVariant(const Type& b, bool constant = false): ref(std::cref(b)), constant(constant){}
      template<UniformType Type>
      UniformRefVariant(Type&& b): ref(std::cref(b)), constant(true){}
   };

public:

   // 调用者可以在外部构造参数
   using UniformParam = std::pair<std::string, UniformRefVariant>;
   using TextureParam = std::tuple<GLint, std::string, std::reference_wrapper<const Texture2D>>;

   DrawUnit(
      const VertexArray& vao,
      Program& program, 
      std::vector<UniformParam> uniforms,
      std::vector<TextureParam> textures,
      GLenum mode = GL_TRIANGLES
   ):
      vao(&vao), program(&program), 
      mode(mode), isEnable(true),
      AutoLoader<DrawUnit>(getRefContainer())
   {
      std::set<std::string> uniformSet;
      std::set<int> unitSet;
      for(auto& [name, ref]: uniforms){
         if(uniformSet.contains(name)){
            throwError("pass multiple uniform with same name");
         }
         if(ref.constant){
            std::visit([this, &name](auto ref){
               this->uniforms.emplace_back(name, this->constUniforms.size());
               this->constUniforms.emplace_back(ref);
            }, ref.ref);
         }else{
            std::visit([this, &name](auto ref){
               this->uniforms.emplace_back(name, &ref.get());
            }, ref.ref);
         }
         
      }
      for(auto& [unit, name, ptr] : textures){
         if(uniformSet.contains(name)){
            throwError("pass multiple texture with same sampler name");
         }
         if(unitSet.contains(unit)){
            throwError("pass multiple texture with same unit");
         }
         this->textures.emplace_back(unit, name, &ptr.get());
      }
   }

   DrawUnit(DrawUnit&& drawUnit) = default;
   DrawUnit& operator=(DrawUnit&& drawUnit) = default;
   // 拷贝没啥意义，反正是一模一样的数据
   DrawUnit& operator=(const DrawUnit&) = delete;
   DrawUnit(const DrawUnit&) = delete;

   void setUniforms(){
      for(auto& [name, ptr]: uniforms){
         std::visit([this, &name](auto ptr){
            if constexpr (std::same_as<decltype(ptr), std::size_t>){
               std::visit([this, &name](auto& value){
                  program->setUniform(name, value);
               }, constUniforms[ptr]);
            }else{
               program->setUniform(name, *ptr);
            }
         }, ptr);
      }
   }
   void setTexture(){
      for(auto& [unit, name, ptr]: textures){
         TextureUnit::getInstance().bindUnit(unit, *ptr);
         program->setUniform(name, unit);
      }
   }
   
   void draw(){
      if(isEnable){
         VertexArrayContext::getInstance().bindContext(*vao);
         ProgramContext::getInstance().bindContext(*program);
         setUniforms();
         setTexture();
         checkGLError();
         
         if(vao->isBindEBO()){
            // 开始渲染，绘制三角形，索引数量为6（6/3=2个三角形），偏移为0（如果vao上下文没有绑定ebo则为数据的内存指针）
            glDrawElements(mode, vao->getNumber(), GL_UNSIGNED_INT, 0);
         }else{
            glDrawArrays(mode, 0, vao->getNumber());
         }
      }
   }

   void enable(){
      isEnable = true;
   }
   void disable(){
      isEnable = false;
   }
};

class Drawer: public ProactiveSingleton<Drawer>{
private:
   RefContainer<DrawUnit> drawUnits;
   // 渲染宽高
   ObservableValue<int> width;
   ObservableValue<int> height;

   std::optional<ReactiveBinder<int, int>> reactiveWidth;
   std::optional<ReactiveBinder<int, int>> reactiveHeight;

   GuiDrawer guiDrawer;

   class SizeObserver: public AbstractValueObserver<int, int>{
   private:
      void handle(const int& width, const int& height) override {
         // 设置opengl渲染在窗口中的起始位置和大小
         glViewport(0, 0, width, height);
      }
   public:
      SizeObserver(const ObservableValue<int>& width, const ObservableValue<int>& height): 
         AbstractValueObserver<int, int>(width, height){}
   };

   SizeObserver sizeObserver;

public: 
   Drawer():
      width(Context::getInstance().getWidth().get()),
      height(Context::getInstance().getHeight().get()),
      sizeObserver(width, height),
      reactiveWidth(std::in_place, [](auto width){return width;}, this->width, Context::getInstance().getWidth()),
      reactiveHeight(std::in_place, [](auto height){return height;}, this->height, Context::getInstance().getHeight())
   {
      // 设置opengl渲染在窗口中的起始位置和大小
      glViewport(0, 0, width, height);

      // 开启深度测试
      glEnable(GL_DEPTH_TEST);
   }
   void draw();

   RefContainer<DrawUnit>& getDrawUnitContainer() { return drawUnits; }
};



inline void Drawer::draw(){
   // 设置清除缓冲区的颜色并清除缓冲区
   glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
   // 同时清除颜色缓冲区和深度缓冲区
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   for(auto& drawUnit: drawUnits){
      drawUnit.draw();
   }
   guiDrawer.draw();
   glfwSwapBuffers(Context::getInstance().getWindow());
   checkGLError();
}

inline RefContainer<DrawUnit>& DrawUnit::getRefContainer() {
   return Drawer::getInstance().getDrawUnitContainer();
}





};
#endif // _MINECPP_RESOURCE_H_
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
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tool.hpp"
#include <type_traits>

namespace minecpp{

// opengl中的一些具有 GLuind 类型id的对象
enum class ResourceType{
   BUFFER, VERTEXARRAY, TEXTURE, PROGRAM, SHADER
};

// opengl中的一些具有GLuind 类型id的对象的模板生成方法
// 一些公共特征：
// GLuint 作为id 的对象
// 只能构造和移动，无法拷贝
// subType：一些资源的子类型，如buffer还有 vbo, ebo
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

// 根据不同 resource type 设置 createResource 函数特化
template<ResourceType type, GLenum subType>
GLuint GLResource<type, subType>::createResource() {
   // todo 添加静态类型断言
   if constexpr(type == ResourceType::BUFFER){
      GLuint id;
      glGenBuffers(1, &id);
      return id;
   }else if constexpr(type == ResourceType::VERTEXARRAY){
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
      // 如果直接使用static_assert，则不管编译后还有没有这个blcok，还是会报错
      // 如果将static_assert包装在lambda中调用，则只有进来这个block后才会进行实例化，从而才会报错
      []<bool flag = false>(){static_assert(flag);}();
   }
}

// 同上
template<ResourceType type, GLenum subType>
void GLResource<type, subType>::deleteResource(GLuint id) {
   // todo 添加静态类型断言
   if constexpr(type == ResourceType::BUFFER){
      glDeleteBuffers(1, &id);
   }else if constexpr(type == ResourceType::VERTEXARRAY){
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

// opengl中的一些需要绑定 resource 的 context target
enum class ContextType{
   BUFFER, VERTEXARRAY, TEXTURE, PROGRAM
};

// opengl中的一些 context target 绑定函数
template<ContextType type, GLenum subType>
void bind_(GLuint resourceId) noexcept {
   if constexpr(type == ContextType::BUFFER){
      glBindBuffer(subType, resourceId);
   }else if constexpr(type == ContextType::VERTEXARRAY){
      glBindVertexArray(resourceId);
   }else if constexpr(type == ContextType::TEXTURE){
      glBindTexture(subType, resourceId);
   }else if constexpr(type == ContextType::PROGRAM){
      glUseProgram(resourceId);
   }else{
      []<bool flag = false>(){static_assert(flag);}();
   }
}

template<ResourceType resource>
constexpr ContextType rse2Ctx(){
   if constexpr (resource == ResourceType::BUFFER){
      return ContextType::BUFFER;
   }else if constexpr (resource == ResourceType::VERTEXARRAY){
      return ContextType::VERTEXARRAY;
   }else if constexpr (resource == ResourceType::PROGRAM){
      return ContextType::PROGRAM;
   }else if constexpr (resource == ResourceType::TEXTURE){
      return ContextType::TEXTURE;
   }else{
      []<bool flag = false>(){static_assert(flag);}();
   }
}

// 对于某个 context target，记录当前的目标，如果等于要绑定的目标，就不用重新执行绑定函数
// 如果实际执行了一次绑定，则返回 true
template<ContextType type, GLenum subType=0>
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
   return bindContext<rse2Ctx<type>(), subType>(resource.getId());
}

template<ResourceType type, GLenum subType=0>
bool mustBindContext(const GLResource<type, subType>& resource) noexcept {
   return bindContext<rse2Ctx<type>(), subType>(resource.getId(), true);
}

// 创建一个指定类型的buffer并设置其为当前上下文，同时绑定数据
template<GLenum bufferType>
class Buffer: public GLResource<ResourceType::BUFFER, bufferType>{
public:
   Buffer(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW){
      bindContext(*this);
      // 设置GL_ARRAY_BUFFER上下文对象的数据
      // GL_STREAM_DRAW：数据只设置一次，GPU最多使用几次
      // GL_STATIC_DRAW：数据只设置一次，使用多次
      // GL_DYNAMIC_DRAW：数据变化很大，使用次数也很多
      glBufferData(bufferType, size, data, usage);
      checkGLError();
   }
};

using VertexBuffer = Buffer<GL_ARRAY_BUFFER>;
using ElementBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER>;

class VertexArray: public GLResource<ResourceType::VERTEXARRAY>{
public:
   void addAttribute(
      const VertexBuffer& buffer,
      GLuint index, 
      GLint size, 
      GLenum type, 
      GLboolean normalized, 
      GLsizei stride, 
      const void * pointer){
         bindContext(*this);
         // 对于 array buffer 上下文目标是全局的
         bindContext(buffer);
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
class Shader: public GLResource<ResourceType::SHADER, shaderType>{
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

class Program: public GLResource<ResourceType::PROGRAM>{
private:
   // 存储 program 中的已知的 uniform 及其 location
   std::map<std::string, GLint> uniforms; 
public:
   // 使用万能引用
   // 如果万能引用实例化为右值引用，则链接成功后则shader对象在program构造结束后会调用析构，即删除shader对象的资源
   template<typename VS, typename FS>
   Program(VS&& vertexShader, FS&& fragmentShader);

   template<typename T>
   void setUniform(const std::string& name, const T& value){
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
class Texture: public GLResource<ResourceType::TEXTURE, textureType>{};

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
   // opengl 默认的unit就是GL_TEXTURE0
   Texture2D(const char* filepath, GLint unit = 0): Texture2D(unit, filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr) {}
   Texture2D(GLint unit, const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, glm::vec4* borderColor);
};

// 在指定的纹理单元中创建2D纹理
// filepath: 图片文件
// horizonType, verticalType: 横向、纵向扩展类型
// 可选 GL_REPEAT, GL_MIRRRED_REREPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
// minFilterType, magFilterType: 纹理缩小/放大过滤方式
// 可选GL_[NEAREST|LINEAR]、 GL_[NEAREST|LINEAR]_MIPMAP_[NEAREST|LINEAR]（仅magFilterType）
// borderColor：当设置为 GL_CLAMP_TO_BORDER 时的颜色，需要大小为4的数组(rgba)
Texture2D::Texture2D(GLint unit, const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, glm::vec4* borderColor){
   GLuint texture = this->id;

   // 一个texture unit 只能对应于着色器中的一个纹理属性（也即一个unit只需要设置一个类型的上下文，设置多个是没有意义的）
   TextureUnit::bind(unit, *this);

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


class DrawUnit{
private:
   VertexArray& vao;
   Program& program;
   std::map<int, Texture2D*> textures;
   std::vector<std::function<void(DrawUnit&)>> uniformSetters;
   GLenum mode;
   GLsizei count;
public:
   DrawUnit(VertexArray& vao, Program& program, GLenum mode, GLsizei count)
      :vao(vao), program(program), mode(mode), count(count){}

   // addUniform函数保证每次draw时program绑定的uniform一定是当前这个值
   template<typename T>
   void addUniform(const std::string& name, T&& value){
      // just test if can normally set
      this->program.setUniform(name, value);
      if constexpr(std::is_lvalue_reference_v<T&&>){
         // fmt::println("uniform name: {}, arg is left value ref", name);
         // 不能将this放入捕获列表中，因为drawunit没有禁止被移动，则移动后的对象中的this就不指向本对象了
         uniformSetters.push_back([&value, name](DrawUnit& unit){
            unit.program.setUniform(name, value);
         });
      }else{
         // 右值接收声明周期短暂的数据
         // fmt::println("uniform name: {}, arg is right value ref", name);
         uniformSetters.push_back([value, name](DrawUnit& unit){
            unit.program.setUniform(name, value);
         });
      }
      
   }
   void addTexture(Texture2D& texture, GLint unit=0){
      textures.insert({unit, &texture});
   }
   void draw(){
      bindContext(vao);
      bindContext(program);
      for(auto& setter: uniformSetters){
         setter(*this);
      }
      for(auto& texture: textures){
         TextureUnit::bind(texture.first, *texture.second);
      }
      glDrawArrays(mode, 0, count);
   }
};

class Context: public ProactiveSingleton<Context>{
friend class InputProcessor;
public:
   const int majorVersion;
   const int minorVersion;
private:
   void createWindow();
   GLFWwindow* window;
   AssignObservable<int> width;
   AssignObservable<int> height;

public:
   Context(int width, int height)
   :ProactiveSingleton<Context>(this),
   majorVersion(3), minorVersion(3),
   width(width),height(height){
      if(glfwInit() != GLFW_TRUE){
         throwError("glfw init failed");
      }
      // 设置opengl版本3.3,类型为core profile
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      createWindow();
   }
   ~Context(){glfwTerminate();}
   
   void startLoop(const std::function<void(void)>& loop){
      while (!glfwWindowShouldClose(window)){
         loop();
      }
   }
   GLFWwindow* getWindow(){return window;}
   ObservableValue<int>& getWidth(){return width;}
   ObservableValue<int>& getHeight(){return height;}
};

void Context::createWindow()
{

   // 创建窗口
   this->window = glfwCreateWindow(*width, *height, "LearnOpenGL", NULL, NULL);
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
   
   checkGLFWError([](){
      glfwTerminate();
   });
}


class InputProcessor: public ProactiveSingleton<InputProcessor>{
private:
   template<typename FuncType, int type=0>
   class Handler: public IdHolder<Handler<FuncType, type>>, public std::function<FuncType>{
   public:
      Handler(const std::function<FuncType>& handler):std::function<FuncType>(handler){}
   };

   using KeyHoldHandler = Handler<void(int)>;
   using KeyReleaseHandler = Handler<void(int)>;
   // using KeyUpHandler = Handler<void()>;
   using KeyDownHandler = Handler<void()>;
   using SizeChangeHandler = Handler<void(int, int)>;
   std::map<int, SizeChangeHandler> sizeChangeHandlers;
   std::map<int, std::chrono::_V2::system_clock::time_point> pressedKeys;
   std::map<int, std::map<int, KeyDownHandler>> keyDownHandlers;
   // std::map<int, std::map<int, KeyUpHandler>> keyUpHandlers;
   std::map<int, std::map<int, KeyReleaseHandler>> keyReleaseHandlers;
   std::map<int, std::map<int, KeyHoldHandler>> keyHoldHandlers;
public:
   InputProcessor():ProactiveSingleton(this) {
      // 设置当窗口尺寸变化时的回调函数
      // 还有很多的回调函数，如处理输入等等；须在创建窗口后、开始渲染前注册回调函数
      glfwSetFramebufferSizeCallback(Context::getInstance().getWindow(), [](GLFWwindow *window, int newWidth, int newHeight){
         auto& processor = InputProcessor::getInstance();
         for(auto& handler: processor.sizeChangeHandlers){
            handler.second(newWidth, newHeight);
         }
      });
      glfwSetKeyCallback(Context::getInstance().getWindow(), [](GLFWwindow *window, int key, int scancode, int action, int mods){
         auto& processor = InputProcessor::getInstance();
         if(action == GLFW_PRESS){
            if(processor.keyDownHandlers.contains(key)){
               for(const auto& handler : processor.keyDownHandlers[key]){
                  handler.second();
               }
            }
            auto startTime = std::chrono::high_resolution_clock::now();
            processor.pressedKeys.insert({key, startTime});
         }else if(action == GLFW_RELEASE){
            // if(processor.keyUpHandlers.contains(key)){
            //    for(const auto& handler : processor.keyUpHandlers[key]){
            //       handler();
            //    }
            // }
            if(processor.keyReleaseHandlers.contains(key)){
               auto startTime = processor.pressedKeys[key];
               auto endTime = std::chrono::high_resolution_clock::now();
               auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
               for(const auto& handler : processor.keyReleaseHandlers[key]){
                  handler.second(duration.count());
               }
            }
            processor.pressedKeys.erase(key);
         }

      });
      // ctx window size change handler
      addSizeChangeHandler([](auto width, auto height){
         auto& ctx = Context::getInstance();
         ctx.height = height;
         ctx.width = width;
      });
   };

private:
   template<typename HandlerType, typename FuncType>
   int addHandler(std::map<int, HandlerType>& map, const std::function<FuncType>& handler){
      HandlerType _handler {handler};
      int id = _handler.getId();
       auto ret = map.insert({id, std::move(_handler)});
      return id;
   }
   template<typename HandlerType>
   void removeHandler(std::map<int, HandlerType>& map, int id){
      if(id == 0){
         map.clear();
      }else{
         map.erase(id);
      }
   }
public:
   int addSizeChangeHandler(const std::function<void(int width, int height)>& handler){
      return addHandler(sizeChangeHandlers, handler);
   }
   void removeSizeChangeHandler(int id=0){
      removeHandler(sizeChangeHandlers, id);
   }
   // 下降沿
   int addKeyDownHandler(int key, const std::function<void()>& handler){
      return addHandler(keyDownHandlers[key], handler);
   }
   void removeKeyDownHandler(int key, int id=0){
      removeHandler(keyDownHandlers[key], id);
   }
   int addKeyHoldHandler(int key, const std::function<void(int milli)>& handler){
      KeyHoldHandler _handler {handler};
      int id = _handler.getId();
      keyHoldHandlers[key].insert({id, std::move(_handler)});
      return id;
      // return addHandler(keyHoldHandlers[key], handler);
   }
   void removeKeyHoldHandler(int key, int id=0){
      removeHandler(keyHoldHandlers[key], id);
   }
   int addKeyReleaseHandler(int key, const std::function<void(int holdMilli)>& handler){
      return addHandler(keyReleaseHandlers[key], handler);
   }
   void removeKeyReleaseHandler(int key, int id=0){
      removeHandler(keyReleaseHandlers[key], id);
   }

   void processInput(){
      glfwPollEvents();

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
               handler.second(duration.count());
            }
         }
      }
   }
};

class KeyHoldHandlerSetter{
private:
   int id;
   int key;
   bool moved = false;
public:
   KeyHoldHandlerSetter(int key, const std::function<void(int milli)>& handler){
      int id = InputProcessor::getInstance().addKeyHoldHandler(key, handler);
      this->id = id;
      this->key = key;
   }
   ~KeyHoldHandlerSetter(){
      if(!moved) {
         InputProcessor::getInstance().removeKeyHoldHandler(key, id);
      }
   }
   // 移动语义
   KeyHoldHandlerSetter(KeyHoldHandlerSetter && setter){
      this->id = setter.id;
      this->key = setter.key;
      setter.moved = true;
   }
};

class Drawer: public ProactiveSingleton<Drawer>{
public: 
   std::vector<std::reference_wrapper<DrawUnit>> drawUnits;
   Drawer():ProactiveSingleton(this){
      auto& ctx = Context::getInstance();
      // 设置opengl渲染在窗口中的起始位置和大小
      glViewport(0, 0, ctx.getWidth().get(), ctx.getHeight().get());
      auto& inputProcessor = InputProcessor::getInstance();
      inputProcessor.addSizeChangeHandler([](auto width, auto height){
         glViewport(0, 0, width, height);
      });

      // 开启深度测试
      glEnable(GL_DEPTH_TEST);
   }
   void addDrawUnit(DrawUnit& drawUnit){
      drawUnits.push_back(drawUnit);
   }
   void draw(const std::function<void(void)>& customDraw = []{}){
      // 设置清除缓冲区的颜色并清除缓冲区
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      // 同时清除颜色缓冲区和深度缓冲区
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      for(auto& drawUnit: drawUnits){
         drawUnit.get().draw();
      }
      customDraw();
      glfwSwapBuffers(Context::getInstance().getWindow());
      checkGLError();
   }
};

};
#endif // _MINECPP_RESOURCE_H_
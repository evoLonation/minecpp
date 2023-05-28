#include "render.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
// 如果多个cpp文件中包含了该头文件，则必须有一个之前要加这个来包含实现
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "../exception.hpp"

namespace render_old{

// 通过glfwGetError函数检查错误
std::optional<std::string> getGlfwError(){
   std::string error;
   char description[512];
   const char *descripPtr = description;
   bool isError = false;
   while (glfwGetError(&descripPtr) != GLFW_NO_ERROR)
   {
      isError = true;
      error.append(description).append("\n");
   }
   if(isError){
      return error;
   }else{
      return std::nullopt;
   }
}

// 通过glGetError函数检查错误
std::optional<std::string> getGlError(){
   std::string error;
   bool isError = false;
   GLenum errorCode;
   while ((errorCode = glGetError()) != GL_NO_ERROR)
   {
      isError = true;
      switch (errorCode)
      {
         case GL_INVALID_ENUM:                  error.append("INVALID_ENUM"); break;
         case GL_INVALID_VALUE:                 error.append("INVALID_VALUE"); break;
         case GL_INVALID_OPERATION:             error.append("INVALID_OPERATION"); break;
         case GL_OUT_OF_MEMORY:                 error.append("OUT_OF_MEMORY"); break;
         case GL_INVALID_FRAMEBUFFER_OPERATION: error.append("INVALID_FRAMEBUFFER_OPERATION"); break;
         // maybe later version
         // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
         // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
      }
      error.append("\n");
   }
   if(isError){
      return error;
   }else{
      return std::nullopt;
   }
}

// 如果出错，则抛出异常
void checkGlfwError_(const char *file, int line)
{
   auto err = getGlError();
   if(err.has_value()){
      throw printError(file, line, err.value());
   }
}

// 如果出错，则抛出异常
void checkGlError_(const char *file, int line)
{
   auto err = getGlfwError();
   if(err.has_value()){
      throw printError(file, line, err.value());
   }
}

// 创建一个指定类型的buffer并设置其为当前上下文，同时绑定数据
Buffer::Buffer(GLenum bufferType, const void* data, GLsizeiptr size, GLenum usage)
:GLResource([](GLuint i){glDeleteBuffers(1, &i);})
{
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
   auto err = getGlError();
   if(err.has_value()){
      glBindBuffer(bufferType, 0);
      glDeleteBuffers(1, &buffer);
      error(err.value());
   }
   this->id = buffer;
}

Shader::Shader(GLenum type, const std::string& str, bool isContent)
:GLResource([](GLuint i){glDeleteShader(i);})
{
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
   // 创建着色器对象
   GLuint shader = glCreateShader(type);
   // 第二个参数是字符串的数量
   const char* c_content = content.c_str();
   glShaderSource(shader, 1, &c_content, nullptr);
   glCompileShader(shader);

   auto err = getGlError();
   if(err.has_value()){
      glDeleteShader(shader);
      error(err.value());
   }

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

   this->id = shader;
}

Program::Program(const std::string& vertexShaderStr, const std::string& fragmentShaderStr, bool isContent)
:GLResource([](GLuint i){glDeleteProgram(i);})
{
   // 链接成功后就可以删除着色器对象的资源，对应这两个对象在program构造结束后的析构
   Shader vertexShader = Shader(GL_VERTEX_SHADER, vertexShaderStr, isContent);
   Shader fragmentShader = Shader(GL_FRAGMENT_SHADER, fragmentShaderStr, isContent);

   // 创建program对象
   GLuint program = glCreateProgram();
   
   // 将shader加入program
   glAttachShader(program, vertexShader.getId()); 
   glAttachShader(program, fragmentShader.getId());

   // 链接program
   glLinkProgram(program);

   auto err = getGlError();
   if(err.has_value()){
      glDeleteProgram(program);
      error(err.value());
   }

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
   
   this->id = program;
}

// 在当前的纹理单元中创建2D纹理
// filepath: 图片文件
// horizonType, verticalType: 横向、纵向扩展类型
// 可选 GL_REPEAT, GL_MIRRRED_REREPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
// minFilterType, magFilterType: 纹理缩小/放大过滤方式
// 可选GL_[NEAREST|LINEAR]、 GL_[NEAREST|LINEAR]_MIPMAP_[NEAREST|LINEAR]（仅magFilterType）
// borderColor：当设置为 GL_CLAMP_TO_BORDER 时的颜色，需要大小为4的数组(rgba)
Texture::Texture(const std::string& filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor)
:GLResource([](GLuint i){glDeleteTextures(1, &i);})
{
   // 创建texture对象并将其绑定到当前texture unit的GL_TEXTURE_2D上下文中
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

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
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
   }
   
   int width, height, nrChannels;
   // opengl的纹理坐标的y轴是从底向上从0到1的，但是stb默认加载是从上向下加载的，因此需要翻转一下y轴
   stbi_set_flip_vertically_on_load(true);
   // 读取图片并返回数据、图片的宽高和通道数
   unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
   if(data == nullptr){
      glDeleteTextures(1, &texture);
      error(fmt::format("load image from {} failed", filepath));
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
      // 源数据的通道数为4，说明类型是rgba
      // png格式通常是rgba
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
   }else{
      glDeleteTextures(1, &texture);
      stbi_image_free(data);
      error(fmt::format("the channel of image is not 3 nor 4, is {}", nrChannels));
   }

   // 释放读取图像的内存
   stbi_image_free(data);
   
   if(   magFilterType == GL_LINEAR_MIPMAP_LINEAR ||
         magFilterType == GL_LINEAR_MIPMAP_NEAREST ||
         magFilterType == GL_NEAREST_MIPMAP_LINEAR ||
         magFilterType == GL_NEAREST_MIPMAP_NEAREST) {
      glGenerateMipmap(GL_TEXTURE_2D);
   }

   auto err = getGlError();
   if(err.has_value()){
      glDeleteTextures(1, &texture);
      error(err.value());
   }

   this->id = texture;
}

// 激活给定的纹理单元，并在其中创建2D纹理
Texture createTexture(GLint unitId, const char* filepath){
   // 默认的unit就是GL_TEXTURE0
   // 一个texture unit 只能对应于着色器中的一个纹理属性（也即一个unit只需要设置一个类型的上下文，设置多个是没有意义的）
   glActiveTexture(GL_TEXTURE0 + unitId);

   return Texture(filepath);
}

// 初始化glfw，创建opengl上下文，设置基本回调函数
Context::Context(int width, int height){
   if(glfwInit() != GLFW_TRUE){
      error("glfw init error\n");
   }

   // 设置opengl版本3.3,类型为core profile
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // 创建窗口
   GLFWwindow *window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
   if (window == nullptr)
   {
      glfwTerminate();
      error("Failed to create GLFW window");
   }

   // 将创建的窗口设置为当前opengl上下文
   glfwMakeContextCurrent(window);


   // 借助 glad加载opengl的函数
   int version = gladLoadGL(glfwGetProcAddress);
   if (version == 0)
   {
      glfwTerminate();
      error("Failed to initialize GLAD");
   }

   fmt::print("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

   // 设置opengl渲染在窗口中的起始位置和大小
   glViewport(0, 0, width, height);

   // 设置当窗口尺寸变化时的回调函数
   // 还有很多的回调函数，如处理输入等等；须在创建窗口后、开始渲染前注册回调函数
   glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height){
      glViewport(0, 0, width, height);
   });
   
   auto err = getGlfwError();
   if(err.has_value()){
      glfwTerminate();
      error(err.value());
   }

   this->window = window;
}
Context::~Context(){
   glfwTerminate();
}

Buffer Plane::createEBO(){
   GLuint index4[] = {
      0, 1, 2, 
      0, 2, 3,
   };
   return Buffer(GL_ELEMENT_ARRAY_BUFFER, index4, sizeof(index4), GL_STATIC_DRAW);
}

Buffer Plane::createVBO(){
   GLfloat vertices3[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.5f, 1.5f,   // top right
      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.5f, -1.5f,   // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   -1.5f, -1.5f,   // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   -1.5f, 1.5f    // top left 
   };
   return Buffer(GL_ARRAY_BUFFER, vertices3, sizeof(vertices3), GL_STATIC_DRAW);
}

// 创建一个平面模型，包括：
// vao：包括顶点坐标(0)、顶点颜色(1)、顶点贴图位置(2) 3个顶点属性，一个vbo，最终一共6个顶点
// 创建两个container和awesome-face的2D纹理，纹理单元分别为0和1
// 创建结束后保证vao上下文为该plane的vao
Plane::Plane(): 
vbo(createVBO()), 
ebo(createEBO()),
texture1(createTexture(0, "../image/container.jpg")),
texture2(createTexture(1, "../image/awesome-face.png")){
   
   GLuint vao;
   // 创建一个vao对象
   glGenVertexArrays(1, &vao);
   // 设置vao上下文为刚刚创建的对象，之后会在vao上下文中存储顶点属性和EBO的设置（不直接存储vbo的状态）
   glBindVertexArray(vao);

   // 该函数将当前位于GL_ARRAY_BUFFER上下文的buffer对象的设置为顶点属性
   // index: 对应着色器中的location
   // size: 对应单个属性的元素的个数，比如vec3对应3
   // type: 数据的元素的类型，一般来说和属性的类型保持一致（如果不一致则会进行类型转换），如vec3的基本类型是float，则对应GL_FLOAT
   // normalized: 如果设置为GL_TRUE，那么如果输入的值的类型是整形且对应着色器属性的类型是浮点型，就会映射到[-1, 1]（无符号则为[0, 1]）
   // stride: 步长，多少字节拿一次数据
   // offset：从所给数组的那里开始拿数据
   // 注：这里如果和着色器的类型没有完全对应其实也有可能正常运行
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
   // 启用index对应的顶点属性
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.getId());

   auto err = getGlError();
   if(err.has_value()){
      glDeleteVertexArrays(1, &vao);
      error(err.value());
   }
   
   this->vao = vao;
}
Plane::~Plane(){
   glDeleteVertexArrays(1, &vao);
}

Buffer Cube::createVBO(){
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
   return Buffer(GL_ARRAY_BUFFER, vertices, sizeof(vertices), GL_STATIC_DRAW);
}

// 创建一个正方体模型，包括：
// vao：包括顶点坐标(0)、顶点贴图位置(1) 2个顶点属性，64个顶点
// 创建两个container和awesome-face的2D纹理，纹理单元分别为0和1
// 创建结束后保证vao上下文为该cube的vao
Cube::Cube(): 
vbo(createVBO()), 
texture1(createTexture(0, "../image/container.jpg")),
texture2(createTexture(1, "../image/awesome-face.png")){
   
   GLuint vao;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(0));
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);

   auto err = getGlError();
   if(err.has_value()){
      glDeleteVertexArrays(1, &vao);
      error(err.value());
   }
   
   this->vao = vao;
}
Cube::~Cube(){
   glDeleteVertexArrays(1, &vao);
}

};

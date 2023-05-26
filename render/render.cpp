#include "render.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stb_image.h>

// 当窗口变化时调用的回调函数
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
   glViewport(0, 0, width, height);
}

GLuint vao;
GLuint vbo;
GLuint ebo;
GLuint texture1;
GLuint texture2;
// 创建一个平面模型，包括：
// vao：包括顶点坐标(0)、顶点颜色(1)、顶点贴图位置(2) 3个顶点属性，一个vbo，最终一共6个顶点
// 创建两个container和awesome-face的2D纹理，纹理单元分别为0和1
bool createPlane(){
   texture1 = createTexture(0, "../image/container.jpg");
   if(!texture1){
      printf("create texture1 error\n");
      return false;
   }
   texture2 = createTexture(1, "../image/awesome-face.png");
   if(!texture2){
      printf("create texture2 error\n");
      return false;
   }
   GLfloat vertices3[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   3.0f, 3.0f,   // top right
      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   3.0f, -2.0f,   // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   -2.0f, -2.0f,   // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   -2.0f, 3.0f    // top left 
   };
   GLuint index4[] = {
      0, 1, 2, 
      0, 2, 3,
   };
   
   // 创建一个vao对象
   glGenVertexArrays(1, &vao);
   // 设置vao上下文为刚刚创建的对象，之后会在vao上下文中存储顶点属性和EBO的设置（不直接存储vbo的状态）
   glBindVertexArray(vao);

   vbo = createBuffer(GL_ARRAY_BUFFER, vertices3, sizeof(vertices3), GL_STATIC_DRAW);
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

   ebo = createBuffer(GL_ELEMENT_ARRAY_BUFFER, index4, sizeof(index4), GL_STATIC_DRAW);

   if(checkGlError()){
      deletePlane();
      return false;
   }
   return true;
}
// 删除通过createPlane函数创建的所有资源
void deletePlane(){
   glDeleteBuffers(1, &ebo);
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(0);
   glDeleteBuffers(1, &vbo);
   glDeleteVertexArrays(1, &vao);
   glDeleteTextures(1, &texture1);
   glDeleteTextures(1, &texture2);
}

// 初始化glfw，创建opengl上下文，设置基本回调函数
GLFWwindow* initialize(int width, int height){
   if(glfwInit() != GLFW_TRUE){
      printf("glfw init error\n");
   }

   // 设置opengl版本3.3,类型为core profile
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // 创建窗口
   GLFWwindow *window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
   if (window == nullptr)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return nullptr;
   }
   // 将创建的窗口设置为当前opengl上下文
   glfwMakeContextCurrent(window);


   // 借助 glad加载opengl的函数
   int version = gladLoadGL(glfwGetProcAddress);
   if (version == 0)
   {
      printf("Failed to initialize GLAD\n");
      return nullptr;
   }
   std::cout << "Loaded OpenGL" << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

   // 设置opengl渲染在窗口中的起始位置和大小
   glViewport(0, 0, width, height);

   // 设置当窗口尺寸变化时的回调函数
   // 还有很多的回调函数，如处理输入等等；须在创建窗口后、开始渲染前注册回调函数
   glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
   
   if(checkGlfwError()){
      glfwTerminate();
      return nullptr;
   }

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
   if(checkGlError()){
      glDeleteBuffers(1, &buffer);
      return 0;
   }
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

   if(checkGlError()){
      glDeleteShader(shader);
      return 0;
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

      std::cout << "compile shader failure: " << logInfo;
      glDeleteShader(shader);
      return 0;
   }
   return shader;
}

GLuint createLinkedProgram(const char* vertexShaderPath, const char* fragmentShaderPath){
   GLuint vertexShader = createCompiledShader(GL_VERTEX_SHADER, vertexShaderPath);
   if(!vertexShader){
      return 0;
   }
   GLuint fragmentShader = createCompiledShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
   if(!fragmentShader){
      glDeleteShader(vertexShader);
      return 0;
   }
   // 创建program对象
   GLuint program = glCreateProgram();
   
   // 将shader加入program
   glAttachShader(program, vertexShader); 
   glAttachShader(program, fragmentShader);

   // 链接program
   glLinkProgram(program);

   // 链接成功后就可以删除着色器对象了
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   if(checkGlError()){
      glDeleteProgram(program);
      return 0;
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

      std::cout << "link program failure: " << logInfo;

      glDeleteProgram(program);
      return 0;
   }
   
   return program;
}

GLuint createTexture(GLint unitId, const char* filepath){
   return createTexture(unitId, filepath, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST, nullptr);
}

// 激活给定的纹理单元，并在其中创建2D纹理
// unitId: 要创建纹理的unit
// filepath: 图片文件
// horizonType, verticalType: 横向、纵向扩展类型
// 可选 GL_REPEAT, GL_MIRRRED_REREPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
// minFilterType, magFilterType: 纹理缩小/放大过滤方式
// 可选GL_[NEAREST|LINEAR]、 GL_[NEAREST|LINEAR]_MIPMAP_[NEAREST|LINEAR]（仅magFilterType）
// borderColor：当设置为 GL_CLAMP_TO_BORDER 时的颜色，需要大小为4的数组(rgba)
GLuint createTexture(GLint unitId, const char* filepath, GLenum horizonType, GLenum verticalType, GLenum minFilterType, GLenum magFilterType, const float* borderColor){
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
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizonType);	
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalType);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterType);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterType);

   // 设置GL_CLAMP_TO_BORDER参数后，需要额外自定义颜色
   if(horizonType == GL_CLAMP_TO_BORDER || verticalType == GL_CLAMP_TO_BORDER){
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
   }

   if(checkGlError()){
      glDeleteTextures(1, &texture);
      return 0;
   }
   
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
      glDeleteTextures(1, &texture);
      stbi_image_free(data);
      return 0;
   }
   
   if(   magFilterType == GL_LINEAR_MIPMAP_LINEAR ||
         magFilterType == GL_LINEAR_MIPMAP_NEAREST ||
         magFilterType == GL_NEAREST_MIPMAP_LINEAR ||
         magFilterType == GL_NEAREST_MIPMAP_NEAREST) {
      glGenerateMipmap(GL_TEXTURE_2D);
   }

   if(checkGlError()){
      glDeleteTextures(1, &texture);
      stbi_image_free(data);
      return 0;
   }
   
   // 释放读取图像的内存
   stbi_image_free(data);
   
   return texture;
}

// 通过glGetError函数检查错误
// 如果出错，则返回true
bool checkGlError_(const char *file, int line)
{
   bool isError;
   GLenum errorCode;
   while ((errorCode = glGetError()) != GL_NO_ERROR)
   {
      isError = true;
      std::string error;
      switch (errorCode)
      {
         case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
         case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
         case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
         case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
         case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
         // maybe later version
         // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
         // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
      }
      std::cout << error << " | " << file << " (" << line << ")" << std::endl;
   }
   return isError;
}


// 通过glfwGetError函数检查错误
// 如果出错，则返回true
bool checkGlfwError_(const char *file, int line)
{
   char description[512];
   const char *descripPtr = description;
   bool isError;
   while (glfwGetError(&descripPtr) != GLFW_NO_ERROR)
   {
      isError = true;
      std::cout << description << " | " << file << " (" << line << ")" << std::endl;
   }
   return isError;
}
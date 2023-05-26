#ifndef RENDER_HPP
#define RENDER_HPP

#include <gl.h>
#include <glfw3.h>
bool createPlane();
void deletePlane();
GLFWwindow* initialize(int width, int height);
GLuint createLinkedProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
GLuint createBuffer(GLenum bufferType, const void* data, GLsizeiptr size, GLenum usage);
GLuint createTexture(GLint unitId, const char* filepath);
GLuint createTexture(
   GLint unitId, 
   const char* filepath, 
   GLenum horizonType, 
   GLenum verticalType, 
   GLenum minFilterType, 
   GLenum magFilterType, 
   const float* borderColor);
bool checkGlError_(const char *file, int line);
// __FILE__和__LINE__宏是预定义的，用于表示当前的文件名称和行数
#define checkGlError() checkGlError_(__FILE__, __LINE__) 
bool checkGlfwError_(const char *file, int line);
#define checkGlfwError() checkGlfwError_(__FILE__, __LINE__) 
#endif

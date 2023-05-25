#include <gl.h>
#include <string>
GLuint createLinkedProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
GLuint createBuffer(GLenum bufferType, const void* data, GLsizeiptr size, GLenum usage);
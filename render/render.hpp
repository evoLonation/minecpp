#ifndef RENDER_HPP
#define RENDER_HPP

#include <gl.h>
#include <glfw3.h>

GLFWwindow* initialize(int width, int height);
GLuint createLinkedProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
GLuint createBuffer(GLenum bufferType, const void* data, GLsizeiptr size, GLenum usage);
GLuint createTexture(GLint unitId, const char* filepath);

#endif

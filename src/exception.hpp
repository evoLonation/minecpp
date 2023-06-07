#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <fmt/format.h>
#include <source_location>
#include <functional>
#include <optional>
#include <GLFW/glfw3.h>
#include <gl.h>

inline std::string printError(const std::string& error, const std::source_location location){
   return fmt::format("file: {} ( {} : {} ) ` {} `: {}",
      location.file_name(),
      location.line(),
      location.column(),
      location.function_name(),
      error
   );
}

inline void throwError(const std::string& error, const std::source_location location = std::source_location::current()){
   throw fmt::format("occur error in file: {}:{}:{} function `{}`:\n{}",
      location.file_name(),
      location.line(),
      location.column(),
      location.function_name(),
      error
   );
}

inline void checkError(
std::function<std::optional<std::string>()> errorGetter,
std::function<void(void)> finalizer = nullptr,
const std::source_location location = std::source_location::current()){
   auto error = errorGetter();
   if(error.has_value()){
      if(finalizer != nullptr)finalizer();
      throwError(error.value(), location);
   }
}

inline std::optional<std::string> getGlfwError(){
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

inline std::optional<std::string> getGlError(){
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

inline void checkGLError(
std::function<void(void)> finalizer = nullptr,
const std::source_location location = std::source_location::current()
){
   checkError(getGlError, finalizer, location);
}
inline void checkGLFWError(
std::function<void(void)> finalizer = nullptr,
const std::source_location location = std::source_location::current()
){
   checkError(getGlfwError, finalizer, location);
}


#endif
#ifndef _MINECPP_IMGUI_H_
#define _MINECPP_IMGUI_H_

#include <GLFW/glfw3.h>
#include "resource.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace minecpp
{

// need context first   
class GuiContext{
public:
   GuiContext(){
      auto& ctx = Context::getInstance();
      // imgui 准备阶段 : 
      // call ImGui::CreateContext()
      // call ImGui_ImplXXXX_Init() for each backend
      ImGui::CreateContext();
      // install_callbacks: 如果为true，则imgui会设置glfw的相关callback, 原来的callback（如果有）的函数指针会保存，imgui的callback会先调用
      ImGui_ImplGlfw_InitForOpenGL(ctx.getWindow(), true);
      std::string glslVersion = fmt::format("#version {}{}0", ctx.majorVersion, ctx.minorVersion);
      ImGui_ImplOpenGL3_Init(glslVersion.c_str());
   }
   ~GuiContext(){
      // 销毁阶段: 
      // call ImGui_ImplXXXX_Shutdown() for each backend 
      // call ImGui::DestroyContext()
      ImGui_ImplGlfw_Shutdown();
      ImGui_ImplOpenGL3_Shutdown();
      ImGui::DestroyContext();
   }
};

// need gui context first
class GuiFrame{
public:
   GuiFrame(){
      // frame 创建阶段: 
      // call ImGui_ImplXXXX_NewFrame() for each backend
      // call ImGui::NewFrame() 
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
   }
   
   void render(){
      // render 阶段:  call ImGui::Render() and call ImGui_ImplXXXX_RenderDrawData() for Renderer backend
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   }
};

} // namespace minecpp


#endif // _MINECPP_IMGUI_H_
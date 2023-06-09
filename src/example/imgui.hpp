#ifndef _MINECPP_IMGUI_H_
#define _MINECPP_IMGUI_H_

#include<imgui/imgui.h>
#include<imgui/imgui_impl_opengl3.h>
#include<imgui/imgui_impl_glfw.h>
#include"../resource.hpp"
#include<gl.h>
#include<fmt/format.h>

namespace imgui
{

int run(){
   using namespace minecpp;
   auto& ctx = Context::getInstance();
   ctx.createWindow(1280, 720);

   // imgui 准备阶段 : 
   // call ImGui::CreateContext()
   // call ImGui_ImplXXXX_Init() for each backend
   ImGui::CreateContext();
   // install_callbacks: 如果为true，则imgui会设置glfw的相关callback, 原来的callback（如果有）的函数指针会保存，imgui的callback会先调用
   ImGui_ImplGlfw_InitForOpenGL(ctx.getWindow(), true);
   std::string glslVersion = fmt::format("#version {}{}0", ctx.majorVersion, ctx.minorVersion);
   ImGui_ImplOpenGL3_Init(glslVersion.c_str());

   RefDirtyObservable fo = 1.0f;
   auto& f = fo.value();
   fo.addObserver([&]{fmt::println("now f: {}", f);});
   RefDirtyObservable bo = false;
   auto& b = bo.value();
   RefDirtyObservable wo = 300.0f;
   RefDirtyObservable ho = 300.0f;
   auto& w = wo.value();
   auto& h = ho.value();
   RefDirtyObservable xo = 300.0f;
   RefDirtyObservable yo = 300.0f;
   auto& x = xo.value();
   auto& y = yo.value();
   while (!glfwWindowShouldClose(ctx.getWindow())){
      glfwPollEvents();
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // frame 创建阶段: 
      // call ImGui_ImplXXXX_NewFrame() for each backend
      // call ImGui::NewFrame() 
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // gui构建阶段

      const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      // ImGuiCond_FirstUseEver: Set the variable if the object/window has no persistently saved data (no entry in .ini file)
      if(b){
         ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + x, main_viewport->WorkPos.y + y));
         ImGui::SetNextWindowSize(ImVec2(w, h));
      }
      // push window to the stack and start appending to it
      ImGui::Begin("Dear ImGui Demo");

      // 标题
      ImGui::Text("setting");
      // 勾选框设置 boolean
      ImGui::Checkbox("set width", &b);
      // 使用滑动条设置float      
      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::SliderFloat("h", &h, 0.0f, 1000.0f);
      ImGui::SliderFloat("w", &w, 0.0f, 1000.0f);
      ImGui::SliderFloat("x", &x, 0.0f, 1000.0f);
      ImGui::SliderFloat("y", &y, 0.0f, 1000.0f);
      //pop window from the stack
      ImGui::End();

      // render 阶段:  call ImGui::Render() and call ImGui_ImplXXXX_RenderDrawData() for Renderer backend
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      fo.check();

      glfwSwapBuffers(ctx.getWindow());
   }

   // 销毁阶段: 
   // call ImGui_ImplXXXX_Shutdown() for each backend 
   // call ImGui::DestroyContext()
   ImGui_ImplGlfw_Shutdown();
   ImGui_ImplOpenGL3_Shutdown();
   ImGui::DestroyContext();
   

   return 0;
}

} // namespace imgui

#endif // _MINECPP_IMGUI_H_
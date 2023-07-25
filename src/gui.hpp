#ifndef _MINECPP_GUI_H_
#define _MINECPP_GUI_H_

#include <map>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "resource.hpp"

namespace minecpp
{
   
// need context first   
class GuiContext: public ProactiveSingleton<GuiContext>{
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

void slider(const std::string& name, float& value, const float min = -50, const float max = 50){
   ImGui::SliderFloat(name.c_str(), &value, min, max);
}
void slider(const std::string& name, ChangeableObservable<float>& value, const float min = -50, const float max = 50){
   slider(name, value.val(), min, max);
   value.mayNotice();
}

void slider(const std::string& name, glm::vec3& value, const glm::vec3& min = glm::vec3{-5.0f}, const glm::vec3& max = glm::vec3{5.0f}){
   // 这种用法是正确的，因为一个右值的生命周期是其所在的整个表达式
   ImGui::SliderFloat(fmt::format("{}: {}", name, "x").c_str(), &value.x, min.x, max.x);
   ImGui::SliderFloat(fmt::format("{}: {}", name, "y").c_str(), &value.y, min.y, max.y);
   ImGui::SliderFloat(fmt::format("{}: {}", name, "z").c_str(), &value.z, min.z, max.z);
}
void slider(const std::string& name, ChangeableObservable<glm::vec3>& value, const glm::vec3& min = glm::vec3{-5.0f}, const glm::vec3& max = glm::vec3{5.0f}){
   slider(name, value.val(), min, max);
   value.mayNotice();
}


template<typename T>
void showPopup(T& t, std::map<T, std::string> elements){
   if (ImGui::Button("Select..")){
      ImGui::OpenPopup("my_select_popup");
   }
   ImGui::SameLine();
   ImGui::TextUnformatted(elements[t].c_str());
   if (ImGui::BeginPopup("my_select_popup")){
      ImGui::SeparatorText("Aquarium");
      for(auto& element: elements){
         if(ImGui::Selectable(element.second.c_str())){
            t = element.first;
         }
      }
      ImGui::EndPopup();
   }
}

} // namespace minecpp

#endif // _MINECPP_GUI_H_
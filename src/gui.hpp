#ifndef _MINECPP_GUI_H_
#define _MINECPP_GUI_H_

#include "tool.hpp"
#include <format>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace minecpp
{
   
// need context first   
class GuiContext: public ProactiveSingleton<GuiContext>{
public:
   
   GuiContext(GLFWwindow* window, int majorVersion, int minorVersion){
      // imgui 准备阶段 : 
      // call ImGui::CreateContext()
      // call ImGui_ImplXXXX_Init() for each backend
      ImGui::CreateContext();
      // install_callbacks: 如果为true，则imgui会设置glfw的相关callback, 原来的callback（如果有）的函数指针会保存，imgui的callback会先调用
      ImGui_ImplGlfw_InitForOpenGL(window, true);
      std::string glslVersion = fmt::format("#version {}{}0", majorVersion, minorVersion);
      ImGui_ImplOpenGL3_Init(glslVersion.c_str());
   }
   ~GuiContext(){
      // 销毁阶段: 
      // call ImGui_ImplXXXX_Shutdown() for each backend 
      // call ImGui::DestroyContext()
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
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

inline void slider(const std::string& name, float& value, const float min = -50, const float max = 50){
   ImGui::SliderFloat(name.c_str(), &value, min, max);
}
inline void slider(const std::string& name, ObservableValue<float>& value, const float min = -50, const float max = 50){
   slider(name, value.get(), min, max);
   value.mayNotify();
}

inline void slider(const std::string& name, glm::vec3& value, const glm::vec3& min = glm::vec3{-5.0f}, const glm::vec3& max = glm::vec3{5.0f}){
   // 这种用法是正确的，因为一个右值的生命周期是其所在的整个表达式
   ImGui::SliderFloat(fmt::format("{}: {}", name, "x").c_str(), &value.x, min.x, max.x);
   ImGui::SliderFloat(fmt::format("{}: {}", name, "y").c_str(), &value.y, min.y, max.y);
   ImGui::SliderFloat(fmt::format("{}: {}", name, "z").c_str(), &value.z, min.z, max.z);
}
inline void slider(const std::string& name, ObservableValue<glm::vec3>& value, const glm::vec3& min = glm::vec3{-5.0f}, const glm::vec3& max = glm::vec3{5.0f}){
   slider(name, value.get(), min, max);
   value.mayNotify();
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

class GuiUnit{
   virtual void draw() = 0;
};

class GuiWindow: public GuiUnit, public AutoLoader<GuiWindow>{
private:
   RefOrConst<std::string> title;
   RefOrConst<bool> open;

   RefContainer<GuiWindow>& getContainer();
public:
   GuiWindow(RefOrConst<std::string> title, RefOrConst<bool> open):
      AutoLoader<GuiWindow>(getContainer()),
      title(std::move(title)), open(std::move(open)) {}
  void draw() override final {
      if(ImGui::Begin(title.get().c_str(), &open.get())){
         insideDraw();
      }
      ImGui::End();
   }
   virtual void insideDraw() {};

   std::string getTitle() const { return title; }
   bool getOpen() const { return open; }
};


class GuiDrawer: public ProactiveSingleton<GuiDrawer>{
private:
   friend GuiWindow;
   RefContainer<GuiWindow> windows;
public:
   void draw(){
      GuiFrame frame;
      for(auto& window: windows){
         window.draw();
      }
      frame.render();
   }
};

inline RefContainer<GuiWindow>& GuiWindow::getContainer() {
   return GuiDrawer::getInstance().windows;
}

} // namespace minecpp

#endif // _MINECPP_GUI_H_
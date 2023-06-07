#include<imgui/imgui.h>
#include<imgui/imgui_impl_opengl3.h>
#include<imgui/imgui_impl_glfw.h>
#include"../resource.hpp"

namespace imgui
{

int run(){
   using namespace render_template2;
   auto& ctx = Context::getInstance();
   ctx.createWindow(1280, 720);
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
   ImGui::StyleColorsDark();
   ImGui_ImplGlfw_InitForOpenGL(ctx.getWindow(), false);
   ImGui_ImplOpenGL3_Init("#version 330");
   bool show_demo_window = true;
   while (!glfwWindowShouldClose(ctx.getWindow())){
      glfwPollEvents();
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      ImGui::ShowDemoWindow(&show_demo_window);
      ImGui::Render();
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      glfwSetKeyCallback(ctx.getWindow(), [](GLFWwindow *window, int key, int scancode, int action, int mods){});

      glfwSwapBuffers(ctx.getWindow());
   }
}

} // namespace imgui



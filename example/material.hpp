#include <iostream>
#include "../render/resource.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace material
{

GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

int run()
{  
   using namespace render_template2;
   try{
      auto& context = Context::getInstance();
      context.createWindow(800, 600);
      auto& inputProcessor = InputProcessor::getInstance();
      DirtyObservable width = 800;
      DirtyObservable height = 600;
      inputProcessor.addKeyDownHandler(GLFW_KEY_ESCAPE, [&](){
         glfwSetWindowShouldClose(context.getWindow(), true);
      });

      VertexBuffer vbo {vertices, sizeof(vertices), GL_STATIC_DRAW};
      VertexArray vao;
      vao.addAttribute(vbo, 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(0));
      vao.addAttribute(vbo, 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

      Program cubeProgram {
         VertexShader::fromFile("../shader/material/cube.vertex.glsl"),
         FragmentShader::fromFile("../shader/material/cube.frag.glsl")
      };
      Program lightProgram {
         VertexShader::fromFile("../shader/material/light.vertex.glsl"),
         FragmentShader::fromFile("../shader/material/light.frag.glsl")
      };

      // cube model
      DirtyObservable cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
      cubeModel = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * cubeModel.value();
      // light model, light pos
      DirtyObservable lightPos = glm::vec3(3.0f, 0.0f, -3.0f);
      DirtyObservable lightModel = glm::translate(glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), lightPos.value());
      lightPos.addObserver([&](){
         lightModel = glm::translate(glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), lightPos.value());
      });
      // view, view pos
      DirtyObservable viewPos = glm::vec3(1.0f, 0.0f, 1.0f);
      DirtyObservable view = glm::lookAt(viewPos.value(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      view.addObserver([&](){
         viewPos = glm::vec3(glm::inverse(view.value())[3]);
         // viewPos = glm::vec3(glm::inverse(view.value()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
      });
      // projection
      DirtyObservable projection = glm::perspective(glm::radians(45.0f), width.value() * 1.0f / height.value() , 0.1f, 100.0f);
      auto computePj = [&](){
         projection = glm::perspective(glm::radians(45.0f), width.value() * 1.0f / height.value() , 0.1f, 100.0f);
      };
      width.addObserver(computePj);
      height.addObserver(computePj);

      // normal model
      DirtyObservable normalModel = glm::mat3(glm::transpose(glm::inverse(cubeModel.value())));
      cubeModel.addObserver([&](){
         normalModel = glm::mat3(glm::transpose(glm::inverse(cubeModel.value())));
      });
      DirtyObservable cubeColor = glm::vec3(1.0f, 0.5f, 0.31f);
      DirtyObservable lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
      

      checkGLError();

      DrawUnit cube  {vao, cubeProgram};
      DrawUnit light {vao, lightProgram};

      // cube: material
      auto ambient = glm::vec3(1.0f, 0.5f, 0.31f);
      auto diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
      auto specular = glm::vec3(0.5f, 0.5f, 0.5f);
      auto shininess = 32.0f;
      cube.addUniform("material.ambient", ambient);
      cube.addUniform("material.diffuse", diffuse);
      cube.addUniform("material.specular", specular);
      cube.addUniform("material.shininess", shininess);
      // cube: light
      auto lightdiffuse = lightColor.value() * glm::vec3(0.5f);
      auto lightambient = lightdiffuse * glm::vec3(0.2f);
      lightColor.addObserver([&](){
         lightdiffuse = lightColor.value() * glm::vec3(0.5f);
         lightambient = lightdiffuse * glm::vec3(0.2f);
      });
      auto lightspecular = glm::vec3(1.0f, 1.0f, 1.0f);
      cube.addUniform("light.ambient", lightambient);
      cube.addUniform("light.diffuse", lightdiffuse);
      cube.addUniform("light.specular", lightspecular);
      cube.addUniform("light.position", lightPos.value());

      // cube: normal model, color, light color, light pos, view pos
      cube.addUniform("normalModel", normalModel.value());
      cube.addUniform("color", cubeColor.value());
      cube.addUniform("viewPos", viewPos.value());

      // model, view, projection
      cube.addUniform("model", cubeModel.value());
      light.addUniform("model", lightModel.value());
      cube.addUniform("view", view.value());
      light.addUniform("view", view.value());
      cube.addUniform("projection", projection.value());
      light.addUniform("projection", projection.value());
      
      // light: color
      light.addUniform("color", lightColor.value());


      auto updateView = [&](const glm::mat4& view){
         glUseProgram(cubeProgram.getId());
         glUniformMatrix4fv(glGetUniformLocation(cubeProgram.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
         // 同时改变viewPos，世界空间中的原点经过view变换为相机空间中的位置a，那么相机在世界中的位置就是-a
         glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "viewPos"), 1, glm::value_ptr(glm::vec3(glm::inverse(view)[3])));
         glUniform3fv(glGetUniformLocation(cubeProgram.getId(), "viewPos"), 1, glm::value_ptr(glm::vec3(glm::inverse(view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
         glUseProgram(lightProgram.getId());
         glUniformMatrix4fv(glGetUniformLocation(lightProgram.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
      };
      auto moveSpeed = 0.05f;
      inputProcessor.addKeyHoldHandler(GLFW_KEY_A, [&](auto _){
         view = glm::translate(glm::vec3(moveSpeed, 0.0f, 0.0f)) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_D, [&](auto _){
         view = glm::translate(glm::vec3(-moveSpeed, 0.0f, 0.0f)) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_Z, [&](auto _){
         view = glm::translate(glm::vec3(0.0f, -moveSpeed, 0.0f)) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_X, [&](auto _){
         view = glm::translate(glm::vec3(0.0f, moveSpeed, 0.0f)) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_W, [&](auto _){
         view = glm::translate(glm::vec3(0.0f, 0.0f, moveSpeed)) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_S, [&](auto _){
         view = glm::translate(glm::vec3(0.0f, 0.0f, -moveSpeed)) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_L, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_J, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f))) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_I, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f))) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_K, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f))) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_U, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f))) * view.value();
      });
      inputProcessor.addKeyHoldHandler(GLFW_KEY_O, [&](auto window){
         view = glm::mat4_cast(glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f))) * view.value();
      });

      

      while (!glfwWindowShouldClose(context.getWindow()))
      {
         
         // 设置清除缓冲区的颜色并清除缓冲区
         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
         // 同时清除颜色缓冲区和深度缓冲区
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         cube.draw();
         glDrawArrays(GL_TRIANGLES, 0, 64);
         light.draw();
         glDrawArrays(GL_TRIANGLES, 0, 64);

         lightPos = glm::rotate(lightPos.value(), glm::radians(0.02f), glm::vec3(0.0f, 1.0f, 0.0f));
         lightColor = glm::vec3(sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f));
         glfwSwapBuffers(context.getWindow());
         glfwPollEvents();
         inputProcessor.processInput();
         checkGLError();
      }
   }catch(std::string exp){
      std::cout << exp << std::endl;
      return 1;
   }
   return 0;
}

} // namespace lighting_3d_2

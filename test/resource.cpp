#include "../src/tool.hpp"
#include "../src/resource.hpp"
#include "../src/vertex.hpp"
#include "fmt/core.h"
#include "../src/light.hpp"

#include <gtest/gtest.h>

inline std::array<std::tuple<glm::vec3, glm::vec3, glm::vec2>, 36> vertices = {
   // positions          // normals           // texture coords
   std::tuple{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  0.0f}},

   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  0.0f}},

   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},

   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},

   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},

   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}}
};

// 测试同一个 EBO 能否绑定到多个 VAO
TEST(resource, 1) {
    using namespace minecpp;
    try{
    Context ctx {1920, 1080};
    InputProcessor processor;
    Drawer drawer;

    VertexMeta<false, glm::vec3, glm::vec3, glm::vec2> meta {
        .vertexes {::vertices.begin(), ::vertices.end()}
    };

    // for(int i = 0; i < 36; i++){
    //     meta.indices.push_back(i);
    // }

    // meta.indices.resize(meta.indices.size() / 2);
    VertexData vertexData {createVertexData(meta)};
      
    Texture2D texture {"../image/container2.png"};

    BasicData basicData{
        .viewModel = newViewModel(glm::vec3(3.0f, 0.0f, 3.0f)),
    };

    Program program {
        VertexShader::fromFile("../test/shader/vertex.glsl"),
        FragmentShader::fromFile("../test/shader/fragment.glsl"),
    };

    int a = 2;
    DrawUnit drawUnit {
        vertexData.vao, 
        program, 
        {
            {"model", newModel()}, 
            {"view", basicData.viewModel.get()},
            {"projection", basicData.projectionCoord.projection.get()},
        }, 
        {{1, "mTexture", texture}}
    };
    // DrawUnit drawUnit2 {vao2, program};


    // drawUnit.addUniform("model", newModel());
    // drawUnit.addUniform("view", basicData.viewModel.get());
    // drawUnit.addUniform("projection", basicData.projectionCoord.projection.get());
    // drawUnit2.addUniform("model", newModel(glm::vec3{2.0f, 0.0f, 0.0f}));
    // drawUnit2.addUniform("view", basicData.viewModel.get());
    // drawUnit2.addUniform("projection", basicData.projectionCoord.projection.get());

    // drawUnit.addTexture(texture, 0);
    // drawUnit2.addTexture(texture, 0);
    
    ctx.startLoop([&]{
        std::vector<unsigned int> fakeEBO;
        for(int i = 0; i < 36; i++){
           fakeEBO.push_back(1);
        }
        ElementBuffer ebo {fakeEBO};
        GlobalElementBufferContext::getInstance().bindContext(ebo);
        drawer.draw();
        processor.processInput();
    });
    }catch(std::string a){
        fmt::println("{}", a);
    }
}
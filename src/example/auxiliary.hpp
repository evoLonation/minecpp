#ifndef __AUXILIARY_H__
#define __AUXILIARY_H__


#include <fmt/core.h>
#include <string>
#include "../resource.hpp"
#include "../light.hpp"
#include "glm/fwd.hpp"
#include "../gui.hpp"

namespace auxiliary {

using namespace minecpp;

class ModelTransMeta{
public:
    ObservableValue<glm::vec3> scale;
    ObservableValue<glm::vec3> position;
    ObservableValue<glm::vec3> axios;
    ObservableValue<float> angle;
};

class ModelTrans: public ModelTransMeta, public ReactiveValue<glm::mat4, glm::vec3, glm::vec3, glm::vec3, float>{
private:
    using ReactiveModelTrans = ReactiveValue<glm::mat4, glm::vec3, glm::vec3, glm::vec3, float>;

    ReactiveModelTrans createModelTrans(){
        return {[](const glm::vec3& scale, const glm::vec3& position, const glm::vec3& axios, float angle){
            return 
                newModel(position) * 
                glm::rotate(glm::mat4(1.0f), glm::radians(angle), axios) * 
                glm::scale(scale);
        }, scale, position, axios, angle};
    }
public:
    ModelTrans(const glm::vec3 position): 
        ModelTransMeta{.scale = glm::vec3{1.0f, 1.0f, 1.0f}, .position = position, .axios = glm::vec3{1.0f, 1.0f, 1.0f}},
        ReactiveModelTrans(createModelTrans()){}

    void showControllerPanel(){
        slider("object scale", scale, glm::vec3{0.0f}, glm::vec3{5.0f});
        slider("object position", position);
        slider("object axios", axios);
        slider("object angle", angle, 0.0f, 360.f);
    }

    ModelTrans& operator=(const ModelTrans& coord) {
        ModelTrans::operator=(coord);
        ReactiveModelTrans::operator=(createModelTrans());
        return *this;
    }
    ModelTrans(const ModelTrans& coord) :
        ModelTransMeta(coord), ReactiveModelTrans(createModelTrans()){};
    
    ModelTrans& operator=(ModelTrans&& coord) {
        ModelTrans::operator=(std::move(coord));
        ReactiveModelTrans::operator=(createModelTrans());
        return *this;
    }
    ModelTrans(ModelTrans&& coord) :
        ModelTransMeta(std::move(coord)), ReactiveModelTrans(createModelTrans()){};
    

};

inline int run(){
    try{
        Context ctx {1920, 1080};
        InputProcessor ip;
        Drawer drawer;
        GuiContext guiCtx;
        // LightContext lightCtx;
        BasicData basicData;
        // LightScene scene {basicData};
        // std::array<glm::vec3, 8> vertexs {
        //     glm::vec3{0.0f}, glm::vec3{1.0f, 1.0f, 1.0f},
        //     glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f},
        //     glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f},
        //     glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f},
        // };
        std::array<float, 24> vertexs {
            0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        };
        std::array<unsigned int, 6> indices {
            0, 1, 0, 2, 0, 3,
        };
        
        VertexBuffer vbo {vertexs.data(), vertexs.size() * sizeof(float)};
        ElementBuffer ebo {indices.data(), indices.size() * sizeof(unsigned int)};
        VertexArray vao;
        vao.addAttribute(vbo, 0, 3, GL_FLOAT, false, 6 * sizeof(float), (const void*)0);
        vao.addAttribute(vbo, 1, 3, GL_FLOAT, false, 6 * sizeof(float), (const void*)(3 * sizeof(float)));
        vao.bindElementBuffer(ebo);
        
        Program program {
            VertexShader::fromFile("../shader/auxiliary/vertex.glsl"),
            FragmentShader::fromFile("../shader/auxiliary/fragment.glsl"),
        };

        DrawUnit coord {vao, program, 6, GL_LINES};

        ModelTrans modelTrans {glm::vec3{0.0f}};

        coord.addUniform("model", modelTrans.get());
        coord.addUniform("view", basicData.viewModel.get());
        coord.addUniform("projection", basicData.projectionCoord.projection.get());

        ctx.startLoop([&]{
            GuiFrame frame;
            if(ImGui::Begin("controller")){
                modelTrans.showControllerPanel();
            }
            ImGui::End();
            drawer.draw([&]{
                frame.render();
            });
            ip.processInput();
        });

    }catch(std::string a){
        fmt::println("{}", a);
    }

    return 0;
}

};


#endif // __AUXILIARY_H__
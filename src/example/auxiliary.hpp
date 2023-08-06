#ifndef _MINECPP_AUXILIARY_H_
#define _MINECPP_AUXILIARY_H_


#include <cstddef>
#include <cstring>
#include <fmt/core.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include "../resource.hpp"
#include "glm/fwd.hpp"
#include "../gui.hpp"
#include "../transformation.hpp"
#include "../vertex.hpp"
#include "../input.hpp"

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
        ObservableValue<glm::mat4> viewModel {newViewModel(glm::vec3(3.0f, 0.0f, 3.0f))};
        ProjectionCoord projectionCoord;
        ModelMoveSetter modelSetter {viewModel};
        // LightScene scene {basicData};
        glm::vec3 xColor = glm::vec3{1.0f, 0.0f, 0.0f};
        glm::vec3 yColor = glm::vec3{0.0f, 1.0f, 0.0f};
        glm::vec3 zColor = glm::vec3{0.0f, 0.0f, 1.0f};
        VertexMeta<true, glm::vec3, glm::vec3> vertecMeta {
            .vertexes {
                {glm::vec3{0.0f}, xColor},
                {glm::vec3{1.0f, 0.0f, 0.0f}, xColor},
                {glm::vec3{0.0f}, yColor},
                {glm::vec3{0.0f, 1.0f, 0.0f}, yColor},
                {glm::vec3{0.0f}, zColor},
                {glm::vec3{0.0f, 0.0f, 1.0f}, zColor},
            },
            .indices {
                0, 1, 2, 3, 4, 5,
            }
        };
        VertexData<true> vertexData {createVertexData(vertecMeta)};

        ModelTrans modelTrans {glm::vec3{0.0f}};
        Program program{
            VertexShader::fromFile("../shader/auxiliary/vertex.glsl"),
            FragmentShader::fromFile("../shader/auxiliary/fragment.glsl"),
        };
        DrawUnit coord {
            vertexData.vao, 
            program,
            {
                {"model", modelTrans.get()},
                {"view", viewModel.get()},
                {"projection", projectionCoord.projection.get()},
            },
            {},
            GL_LINES
        };

        

        ctx.startLoop([&]{
            // GuiFrame frame;
            // if(ImGui::Begin("controller")){
            //     modelTrans.showControllerPanel();
            // }
            // ImGui::End();
            drawer.draw([&]{
                // frame.render();
            });
            ip.processInput();
        });

    }catch(std::string a){
        fmt::println("{}", a);
    }

    return 0;
}

};



#endif // _MINECPP_AUXILIARY_H_

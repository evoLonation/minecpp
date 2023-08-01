#ifndef __AUXILIARY_H__
#define __AUXILIARY_H__


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

namespace auxiliary {

using namespace minecpp;

namespace test {

template<bool indice, typename... DataTypes>
struct VertexDataMetaBase{
    using Vertex = std::tuple<DataTypes...>;
    std::vector<Vertex> vertexs;
};
template<typename... DataTypes>
struct VertexDataMetaBase<true, DataTypes...>: public VertexDataMetaBase<false, DataTypes...>{
    std::vector<unsigned int> indices;
};

// c++ 17 fold expression
template<typename... DataTypes>
consteval std::size_t getTotalDataSize(){
    return (sizeof(DataTypes) + ...);
}

template<typename... DataTypes>
void fillVertexData(char *arr, DataTypes... dataTypes){
    std::size_t offset = 0;
    (((*reinterpret_cast<DataTypes*>(arr + offset) = dataTypes), offset += sizeof(DataTypes)), ...);
}

// std::tuple 中成员的内存布局并不是声明时的顺序，因此需要转换一下
template<bool indice, typename... DataTypes>
VertexBuffer createVBO(const VertexDataMetaBase<indice, DataTypes...>& meta){
    std::vector<char> vertexs;
    std::size_t stride = getTotalDataSize<DataTypes...>();
    vertexs.resize(meta.vertexs.size() * stride);
    for(int i = 0, j = 0; i < meta.vertexs.size(); i++, j += stride){
        std::apply(fillVertexData<DataTypes...>, std::tuple_cat(std::tuple{vertexs.data() + j}, meta.vertexs[i]));
    }
    glm::vec3 xColor = glm::vec3{1.0f, 0.0f, 0.0f};
    glm::vec3 yColor = glm::vec3{0.0f, 1.0f, 0.0f};
    glm::vec3 zColor = glm::vec3{0.0f, 0.0f, 1.0f};
    return {vertexs};
}

template<typename... DataTypes>
ElementBuffer createEBO(const VertexDataMetaBase<true, DataTypes...>& meta){
    return ElementBuffer {meta.indices};
}

// c++ 17 fold expression
template<typename... DataTypes>
consteval std::size_t getStride(){
    return getTotalDataSize<DataTypes...>();
}

template<typename... DataTypes>
void addAttributes(VertexArray& vao, const VertexBuffer& vbo){
    unsigned int index = 0;
    std::size_t offset = 0;
    std::size_t stride = getStride<DataTypes...>();
    // c++ 17 fold expression
    ((vao.addAttribute<DataTypes>(vbo, index++, stride, offset), offset += sizeof(DataTypes)), ...);
}

template<bool indice>
struct VertexData{
    VertexBuffer vbo;
    VertexArray vao;

    template<typename... DataTypes>
    VertexData(const VertexDataMetaBase<false, DataTypes...>& meta):
        vbo(createVBO(meta)), vao()
    {
        addAttributes<DataTypes...>(vao, vbo);
        vao.setNumber(meta.vertexs.size());
    }
};

template<>
struct VertexData<true>: public VertexData<false>{
    ElementBuffer ebo;

    template<typename... DataTypes>
    VertexData(const VertexDataMetaBase<true, DataTypes...>& meta):
        VertexData<false>(meta), ebo(createEBO(meta))
    {
        vao.bindElementBuffer(ebo);
    }
};

}

enum class VertexType{
    POSITION = 1 << 0, 
    COORD = 1 << 1, 
    NORMAL = 1 << 2,
};

// 任何整数值都可以转换为任何枚举类，即使没有对应的成员
inline constexpr VertexType operator|(VertexType lhs, VertexType rhs){
    return static_cast<VertexType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline constexpr bool operator&(VertexType lhs, VertexType rhs){
    return static_cast<bool>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

template<VertexType vertexType>
struct Vertex;
template<>
struct Vertex<VertexType::POSITION>{
    glm::vec3 position;
};
template<>
struct Vertex<VertexType::POSITION | VertexType::COORD>{
    glm::vec3 position;
    glm::vec2 textureCoord;
};
template<>
struct Vertex<VertexType::POSITION | VertexType::NORMAL>{
    glm::vec3 position;
    glm::vec3 normal;
};
template<>
struct Vertex<VertexType::POSITION | VertexType::COORD | VertexType::NORMAL>{
    glm::vec3 position;
    glm::vec2 textureCoord;
    glm::vec3 normal;
};

template<VertexType vertexType, bool indice>
struct VertexDataMeta;

template<VertexType vertexType>
struct VertexDataMeta<vertexType, false>{
    std::vector<Vertex<vertexType>> vertexs;
};
template<VertexType vertexType>
struct VertexDataMeta<vertexType, true>{
    std::vector<Vertex<vertexType>> vertexs;
    std::vector<unsigned int> indices;
};

template<VertexType vertexType, bool indice>
VertexBuffer createVBO(const VertexDataMeta<vertexType, indice>& meta){
    return VertexBuffer {meta.vertexs};
}
template<VertexType vertexType>
ElementBuffer createEBO(const VertexDataMeta<vertexType, true>& meta){
    return ElementBuffer {meta.indices};
}

consteval std::size_t getStride(VertexType vertexType){
    std::size_t stride = sizeof(glm::vec3);
    if(vertexType & VertexType::COORD){
        stride += sizeof(glm::vec2);
    }
    if(vertexType & VertexType::NORMAL){
        stride += sizeof(glm::vec3);
    }
    return stride;
}

template<VertexType vertexType>
void addAttributes(VertexArray& vao, const VertexBuffer& vbo){
    std::size_t stride = getStride(vertexType);
    unsigned int index = 0;
    std::size_t offset = 0;
    vao.addAttribute<glm::vec3>(vbo, index++, stride, offset);
    offset += sizeof(glm::vec3);
    if constexpr (vertexType & VertexType::NORMAL){
        vao.addAttribute<glm::vec3>(vbo, index++, stride, offset);
        offset += sizeof(glm::vec3);
    }
    if constexpr (vertexType & VertexType::NORMAL){
        vao.addAttribute<glm::vec3>(vbo, index, stride, offset);
    }
}

template<VertexType vertexType>
VertexArray createVAO(const VertexBuffer& vbo, const ElementBuffer& ebo){
    VertexArray vao;
    addAttributes<vertexType>(vao, vbo);
    vao.bindElementBuffer(ebo);
    return vao;
}
template<VertexType vertexType>
VertexArray createVAO(const VertexBuffer& vbo){
    VertexArray vao;
    addAttributes<vertexType>(vao, vbo);
    return vao;
}

template<VertexType vertexType, bool indice>
struct VertexData{};

template<VertexType vertexType>
struct VertexData<vertexType, false>{
    VertexBuffer vbo;
    VertexArray vao;

    VertexData(const VertexDataMeta<vertexType, false>&& meta):
        vbo(createVBO(meta)), vao(createVAO<vertexType>(vbo)){}
};
template<VertexType vertexType>
struct VertexData<vertexType, true>{
    VertexBuffer vbo;
    ElementBuffer ebo;
    VertexArray vao;
    VertexData(const VertexDataMeta<vertexType, true>&& meta):
        vbo(createVBO(meta)), ebo(createEBO(meta)), vao(createVAO<vertexType>(vbo, ebo)){}
};


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
        test::VertexDataMetaBase<false, glm::vec3, glm::vec3> vertecDataMeta {
            .vertexs {
                {glm::vec3{0.0f}, xColor},
                {glm::vec3{1.0f, 0.0f, 0.0f}, xColor},
                {glm::vec3{0.0f}, yColor},
                {glm::vec3{0.0f, 1.0f, 0.0f}, yColor},
                {glm::vec3{0.0f}, zColor},
                {glm::vec3{0.0f, 0.0f, 1.0f}, zColor},
            }
        };
        
        test::VertexData<false> vertexData {vertecDataMeta};

        std::vector<glm::vec3> vertexs {
            glm::vec3{0.0f}, xColor,
            glm::vec3{1.0f, 0.0f, 0.0f}, xColor,
            glm::vec3{0.0f}, yColor,
            glm::vec3{0.0f, 1.0f, 0.0f}, yColor,
            glm::vec3{0.0f}, zColor,
            glm::vec3{0.0f, 0.0f, 1.0f}, zColor,
        };
        VertexBuffer vbo {vertexs};
        VertexArray vao;
        vao.addAttribute<glm::vec3>(vbo, 0, 6 * sizeof(float), 0);
        vao.addAttribute<glm::vec3>(vbo, 1, 6 * sizeof(float), 3 * sizeof(float));
        vao.setNumber(6);
        
        Program program {
            VertexShader::fromFile("../shader/auxiliary/vertex.glsl"),
            FragmentShader::fromFile("../shader/auxiliary/fragment.glsl"),
        };

        DrawUnit coord {vertexData.vao, program, GL_LINES};

        ModelTrans modelTrans {glm::vec3{0.0f}};

        coord.addUniform("model", modelTrans.get());
        coord.addUniform("view", viewModel.get());
        coord.addUniform("projection", projectionCoord.projection.get());

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


#endif // __AUXILIARY_H__
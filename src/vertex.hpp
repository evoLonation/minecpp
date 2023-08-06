#ifndef _MINECPP_VERTEX_H_
#define _MINECPP_VERTEX_H_

#include <tuple>
#include <vector>
#include "resource.hpp"

namespace minecpp {

template<bool index, typename... DataTypes>
struct VertexMeta;

template<typename... DataTypes>
struct VertexMeta<false, DataTypes...>{
    using Vertex = std::tuple<DataTypes...>;
    std::vector<Vertex> vertexes;
};
template<typename... DataTypes>
struct VertexMeta<true, DataTypes...>{
    using Vertex = std::tuple<DataTypes...>;
    std::vector<Vertex> vertexes;
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
template<bool index, typename... DataTypes>
VertexBuffer createVBO(const VertexMeta<index, DataTypes...>& meta){
    std::vector<char> vertexes;
    std::size_t stride = getTotalDataSize<DataTypes...>();
    vertexes.resize(meta.vertexes.size() * stride);
    for(int i = 0, j = 0; i < meta.vertexes.size(); i++, j += stride){
        std::apply(fillVertexData<DataTypes...>, std::tuple_cat(std::tuple{vertexes.data() + j}, meta.vertexes[i]));
    }
    glm::vec3 xColor = glm::vec3{1.0f, 0.0f, 0.0f};
    glm::vec3 yColor = glm::vec3{0.0f, 1.0f, 0.0f};
    glm::vec3 zColor = glm::vec3{0.0f, 0.0f, 1.0f};
    return {vertexes};
}

template<typename... DataTypes>
ElementBuffer createEBO(const VertexMeta<true, DataTypes...>& meta){
    return ElementBuffer {meta.indices};
}

// c++ 17 fold expression
template<typename... DataTypes>
consteval std::size_t getStride(){
    return getTotalDataSize<DataTypes...>();
}

// 不用万能引用是因为 这样就需要在fold expression中多次std::move(vbo)，造成未定义行为
template<typename... DataTypes>
void addAttributes(VertexArray& vao, const VertexBuffer& vbo){
    unsigned int index = 0;
    std::size_t offset = 0;
    std::size_t stride = getStride<DataTypes...>();
    // c++ 17 fold expression
    ((vao.addAttribute<DataTypes>(vbo, index++, stride, offset), offset += sizeof(DataTypes)), ...);
}

template<bool index>
struct VertexData;

template<>
struct VertexData<false>{
    VertexBuffer vbo;
    VertexArray vao;
};
template<>
struct VertexData<true>{
    VertexBuffer vbo;
    ElementBuffer ebo;
    VertexArray vao;
};

template<bool index, typename... DataTypes>
VertexData<index> createVertexData(const VertexMeta<index, DataTypes...>& meta){
    VertexArray vao;
    VertexBuffer vbo {createVBO(meta)};
    addAttributes<DataTypes...>(vao, std::move(vbo));
    if constexpr (index){
        ElementBuffer ebo {createEBO(meta)};
        vao.bindElementBuffer(ebo);
        return {std::move(vbo), std::move(ebo), std::move(vao)};
    }else{
        vao.setNumber(meta.vertexes.size());
        return {std::move(vbo), std::move(vao)};
    }
}

}// minecpp


#endif // _MINECPP_VERTEX_H_

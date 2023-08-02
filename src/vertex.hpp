#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <tuple>
#include <vector>
#include "resource.hpp"

namespace minecpp {

template<bool indice, typename... DataTypes>
struct VertexData;

template<typename... DataTypes>
struct VertexData<false, DataTypes...>{
    using Vertex = std::tuple<DataTypes...>;
    std::vector<Vertex> vertexs;
};
template<typename... DataTypes>
struct VertexData<true, DataTypes...>{
    using Vertex = std::tuple<DataTypes...>;
    std::vector<Vertex> vertexs;
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
VertexBuffer createVBO(const VertexData<indice, DataTypes...>& meta){
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
ElementBuffer createEBO(const VertexData<true, DataTypes...>& meta){
    return ElementBuffer {meta.indices};
}

// c++ 17 fold expression
template<typename... DataTypes>
consteval std::size_t getStride(){
    return getTotalDataSize<DataTypes...>();
}

// 不用万能引用是因为 这样就需要在fold expression中多次std::move(vbo)，造成未定义行为
template<typename... DataTypes>
void addAttributes(VertexArray& vao, std::common_reference_with<VertexBuffer> auto&& vbo){
    unsigned int index = 0;
    std::size_t offset = 0;
    std::size_t stride = getStride<DataTypes...>();
    // c++ 17 fold expression
    if constexpr (sizeof...(DataTypes) == 1){
        ((vao.addAttribute<DataTypes>(std::forward<decltype(vbo)>(vbo), index++, stride, offset), offset += sizeof(DataTypes)), ...);
    }else{
        ((vao.addAttribute<DataTypes>(vbo, index++, stride, offset), offset += sizeof(DataTypes)), ...);
    }
}

template<bool indice, typename... DataTypes>
VertexArray createVertexArray(const VertexData<indice, DataTypes...>& meta){
    VertexArray vao;
    VertexBuffer vbo {createVBO(meta)};
    addAttributes<DataTypes...>(vao, std::move(vbo));
    if constexpr (indice){
        ElementBuffer ebo {createEBO(meta)};
        vao.bindElementBuffer(std::move(ebo));
    }else{
        vao.setNumber(meta.vertexs.size());
    }
    return vao;
}

}// minecpp

#endif // __VERTEX_H__
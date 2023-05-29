#version 330 core

layout (location = 0) in vec3 inPos;
// 法向量
layout (location = 1) in vec3 inNormal;

// 法向量的模型矩阵，没有位移变换
uniform mat3 normalModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;

void main()
{
   normal = normalModel * inNormal;
   vec4 fragPos4 = model * vec4(inPos, 1.0f);
   gl_Position = projection * view * fragPos4;
   fragPos = fragPos4.xyz;
}
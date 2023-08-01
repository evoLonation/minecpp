// 着色器的版本和类型
#version 330 core
// 顶点着色器的目标是设置gl_Position的值，必须将其设置为vec4类型

// 顶点属性，需要与实际声明对应
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 color;

void main()
{
   gl_Position = projection * view * model * vec4(inPos, 1.0);
   color = inColor;
}
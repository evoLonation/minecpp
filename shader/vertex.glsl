// 着色器的版本和类型
#version 330 core
// 顶点着色器的目标是设置gl_Position的值，必须将其设置为vec4类型

// 顶点属性，需要与实际声明对应
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inCoord;

uniform float factor;

out vec3 color;
out vec2 coord;

void main()
{
   // gl_Position = vec4(aPos.x/2147483647.0, aPos.y/2147483647.0, 0, 1.0);
   gl_Position = vec4(inPos.xyz * factor, 1.0);
   color = inColor;
   coord = inCoord;
}
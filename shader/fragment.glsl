#version 330 core

in vec3 aColor;
out vec4 fragColor;

uniform vec4 color;

void main()
{
   fragColor = vec4(aColor, 1.0f);
   // fragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
   fragColor = color;
}
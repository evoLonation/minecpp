#version 330 core

// 从顶点着色器传来的法向量（世界空间）
in vec3 normal;
// 从顶点着色器传来的顶点坐标（世界空间）
in vec3 fragPos;

in vec2 coord;

// 物体本身的颜色
uniform vec3 color;
// 光源的颜色
uniform vec3 lightColor;
// 光源的坐标（世界空间）
uniform vec3 lightPos;
// 摄像机的坐标（世界空间）
uniform vec3 viewPos;

out vec4 fragColor;

uniform sampler2D mTexture1;
uniform sampler2D mTexture2;

void main()
{
   // ambient lighting, 环境光，光源简单的乘以一个系数再加到自己的光上
   float ambientFactor = 0.1f;
   vec3 ambient = ambientFactor * lightColor;
   // diffuse lighting, 漫反射光，是光的方向与片段的法向量的点积
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(lightPos - fragPos);
   // 通过计算片段的法向量与光线方向的点积
   vec3 diffuse = max(dot(norm, lightDir), 0.0f) * lightColor;
   // specular lighting, 镜面反射光，是光从顶点反射过来的方向与顶点到摄像机方向的点积
   vec3 reflectDir = reflect(-lightDir, norm);
   vec3 viewDir = normalize(viewPos - fragPos);
   float specularFactor = 0.5;
   // pow 用于计算 x 的 y 次方
   // 因此后面的数字越大，亮点越集中
   vec3 specular = specularFactor * pow(max(dot(viewDir, reflectDir), 0.0), 64) * lightColor;

   vec3 result = (ambient + diffuse + specular) * color;

   fragColor = mix(mix(texture(mTexture1, coord), texture(mTexture2, coord), 0.4f), vec4(result, 1.0f), 0.3f);
}
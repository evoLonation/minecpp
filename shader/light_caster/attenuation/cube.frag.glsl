#version 330 core

// 从顶点着色器传来的法向量（世界空间）
in vec3 normal;
// 从顶点着色器传来的顶点坐标（世界空间）
in vec3 fragPos;

in vec2 coord;

// 物体材质
struct Material {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
}; 
uniform Material material;

// 灯光数据，包括材质信息和位置等等
struct Light {
   vec3 position;
  
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   // 常量系数
   float constant;
   // 一次系数
   float linear;
   // 二次系数
   float quadratic;
};

uniform Light light;

// 摄像机的坐标（世界空间）
uniform vec3 viewPos;

out vec4 fragColor;

void main()
{
   // 光强度的衰减
   float dist = length(light.position - fragPos);
   float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
   
   vec3 materialDiffuse = vec3(texture(material.diffuse, coord));
   vec3 materialSpecular = vec3(texture(material.specular, coord));
   // ambient lighting
   vec3 ambient =  light.ambient * materialDiffuse;
   // diffuse lighting
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(light.position - fragPos);
   // 通过计算片段的法向量与光线方向的点积
   vec3 diffuse = max(dot(norm, lightDir), 0.0f) * materialDiffuse * light.diffuse;
   // specular lighting, 镜面反射光，是光从顶点反射过来的方向与顶点到摄像机方向的点积
   vec3 reflectDir = reflect(-lightDir, norm);
   vec3 viewDir = normalize(viewPos - fragPos);
   // pow 用于计算 x 的 y 次方
   // 因此后面的数字越大，亮点越集中
   vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * materialSpecular * light.specular;

   // 对各种分量应用衰减
   // 对于单一光源，可以不对ambient（环境光）应用衰减
   ambient  *= attenuation; 
   diffuse  *= attenuation;
   specular *= attenuation;
   
   vec3 result = ambient + diffuse + specular;

   fragColor = vec4(result, 0);
}
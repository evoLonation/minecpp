#version 330 core

// 从顶点着色器传来的法向量（世界空间）
in vec3 normal;
// 从顶点着色器传来的顶点坐标（世界空间）
in vec3 fragPos;

in vec2 coord;

// 物体材质
struct Material {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   float shininess;
}; 
uniform Material material;

// 灯光数据，包括材质信息和位置等等
struct Light {
   vec3 position;
  
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

uniform Light light;

// 物体本身的颜色
uniform vec3 color;
// 摄像机的坐标（世界空间）
uniform vec3 viewPos;

out vec4 fragColor;

void main()
{
   // ambient lighting
   vec3 ambient =  light.ambient * material.ambient;
   // diffuse lighting
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(light.position - fragPos);
   // 通过计算片段的法向量与光线方向的点积
   vec3 diffuse = max(dot(norm, lightDir), 0.0f) * material.diffuse * light.diffuse;
   // specular lighting, 镜面反射光，是光从顶点反射过来的方向与顶点到摄像机方向的点积
   vec3 reflectDir = reflect(-lightDir, norm);
   vec3 viewDir = normalize(viewPos - fragPos);
   // pow 用于计算 x 的 y 次方
   // 因此后面的数字越大，亮点越集中
   vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * material.specular * light.specular;

   vec3 result = (ambient + diffuse + specular) * color;
   fragColor = vec4(result, 0);
}
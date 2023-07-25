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

// 聚光灯
struct SpotLight {
   vec3 position;
   vec3  direction;
   float outerCutOff;
   float innerCutOff;

   // 材质
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   // 衰减系数
   float constant;
   float linear;
   float quadratic;
};

struct PointLight{
   vec3 position;
  
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;
};

// 直射光
struct DirectionalLight {
   vec3 direction;
  
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

#define MAX_NUM_DIRECTIONAL_LIGHT 10
#define MAX_NUM_POINT_LIGHT 10
#define MAX_NUM_SPOT_LIGHT 10

uniform DirectionalLight directionalLights[MAX_NUM_DIRECTIONAL_LIGHT];
uniform SpotLight spotLights[MAX_NUM_SPOT_LIGHT];
uniform PointLight pointLights[MAX_NUM_POINT_LIGHT];
uniform int directionalLightNum;
uniform int spotLightNum;
uniform int pointLightNum;

// 摄像机的坐标（世界空间）
uniform vec3 viewPos;

out vec4 fragColor;

float computeAttenuation(vec3 position, float constant, float linear, float quadratic){
   float dist = length(position - fragPos);
   float attenuation = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
   return attenuation;
}

// 漫反射
vec3 computeDiffuse(vec3 lightDir, vec3 normal, vec3 materialDiffuse, vec3 lightDiffuse){
   // diffuse lighting
   // vec3 norm = normalize(normal);
   // 通过计算片段的法向量与光线方向的点积
   vec3 diffuse = max(dot(normal, lightDir), 0.0f) * materialDiffuse * lightDiffuse;
   return diffuse;
}
vec3 computeAmbient(vec3 materialDiffuse, vec3 lightAmbient){
   // ambient lighting
   vec3 ambient =  lightAmbient * materialDiffuse;
   return ambient;
}

vec3 computeSpecular(vec3 lightDir, vec3 viewDir, vec3 normal, float shininess, vec3 materialSpecular, vec3 lightSpecular){
   // specular lighting, 镜面反射光，是光从顶点反射过来的方向与顶点到摄像机方向的点积
   vec3 reflectDir = reflect(-lightDir, normal);
   // pow 用于计算 x 的 y 次方
   // 因此后面的数字越大，亮点越集中
   vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess) * materialSpecular * lightSpecular;
   return specular;
}

float computeSpotIntensity(float theta, float innerCutOff, float outerCutOff){
   float epsilon   = innerCutOff - outerCutOff;
   float intensity = (theta - outerCutOff) / epsilon;
   return intensity;
}


vec3 computeSpotLight(SpotLight light, vec3 viewDir, vec3 normal, vec3 materialDiffuse, vec3 materialSpecular){
   float attenuation = computeAttenuation(light.position, light.constant, light.linear, light.quadratic);
   vec3 ambient =  computeAmbient(materialDiffuse, light.ambient);
   vec3 lightDir = normalize(light.position - fragPos);
   // 计算 theta 的 cos 值
   float theta = dot(lightDir, -light.direction);
   if(theta > light.outerCutOff){
      vec3 diffuse = computeDiffuse(lightDir, normal, materialDiffuse, light.diffuse);
      vec3 specular = computeSpecular(lightDir, viewDir, normal,  material.shininess, materialSpecular, light.specular);
      
      if(theta < light.innerCutOff){
         float intensity = computeSpotIntensity(theta, light.innerCutOff, light.outerCutOff);
         diffuse *= intensity;
         specular *= intensity;
      }
      vec3 result = (ambient + diffuse + specular) * attenuation;
      return result;
   }else{
      return ambient * attenuation;
   }
}
vec3 computePointLight(PointLight light, vec3 viewDir, vec3 normal, vec3 materialDiffuse, vec3 materialSpecular){
   float attenuation = computeAttenuation(light.position, light.constant, light.linear, light.quadratic);
   vec3 lightDir = normalize(light.position - fragPos);
   vec3 ambient =  computeAmbient(materialDiffuse, light.ambient);
   vec3 diffuse = computeDiffuse(lightDir, normal, materialDiffuse, light.diffuse);
   vec3 specular = computeSpecular(lightDir, viewDir, normal, material.shininess, materialSpecular, light.specular);
   vec3 result = (ambient + diffuse + specular) * attenuation;
   return result;
}
vec3 computeDirectionalLight(DirectionalLight light, vec3 viewDir, vec3 normal, vec3 materialDiffuse, vec3 materialSpecular){
   vec3 ambient =  computeAmbient(materialDiffuse, light.ambient);
   vec3 diffuse = computeDiffuse(light.direction, normal, materialDiffuse, light.diffuse);
   vec3 specular = computeSpecular(light.direction, viewDir, normal, material.shininess, materialSpecular, light.specular);
   vec3 result = ambient + diffuse + specular ;
   return result;
}


void main()
{  
   vec3 materialDiffuse = vec3(texture(material.diffuse, coord));
   vec3 materialSpecular = vec3(texture(material.specular, coord));
   vec3 viewDir = normalize(viewPos - fragPos);
   vec3 norm = normalize(normal);
   vec3 result = vec3(0.0f, 0.0f, 0.0f);
   for(int i = 0; i < directionalLightNum; i++){
      result += computeDirectionalLight(directionalLights[i], viewDir, norm, materialDiffuse, materialSpecular);
   }
   for(int i = 0; i < spotLightNum; i++){
      result += computeSpotLight(spotLights[i], viewDir, norm, materialDiffuse, materialSpecular);
   }
   for(int i = 0; i < pointLightNum; i++){
      result += computePointLight(pointLights[i], viewDir, norm, materialDiffuse, materialSpecular);
   }
   fragColor = vec4(result, 1.0);
}
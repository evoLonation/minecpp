#ifndef _MINECPP_MODEL_H_
#define _MINECPP_MODEL_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../resource.hpp"
#include "../gui.hpp"
#include "../light.hpp"
#include "../transformation.hpp"


namespace model
{
using namespace minecpp;

class Model;

class Mesh{
private:
   VertexBuffer vbo;
   VertexArray vao;
   ElementBuffer ebo;

   Model& model;
   int materialIndex;

   int number;
public:
   struct LightObjectMetaConstructor{
      VertexArray& vao;
      ElementBuffer& ebo;
      Texture2D& diffuseTexture;
      Texture2D& specularTexture;
      const ObservableValue<glm::mat4>& model;
   };
public:
   Mesh(VertexBuffer&& vbo, VertexArray&& vao, ElementBuffer&& ebo, Model& model, int materialIndex, int number)
   :vbo(std::move(vbo)), vao(std::move(vao)), ebo(std::move(ebo)), model(model), materialIndex(materialIndex), number(number){}
   operator LightObjectMeta();
};

// 禁止移动与拷贝： mesh 含有对 mode 的引用成员，如要移动，则需要将引用变为指针，并且在移动时改变地址
class Model : UnCopyMoveable{
friend class Mesh;
private:
   float shininess;
   struct Material{
      Texture2D diffuse;
      Texture2D specular;
   };
   std::vector<Material> materials;
   std::vector<Mesh> meshes;
   ChangeableObservable<glm::mat4> modelTrans;

   
   int getMaterialIndex(int materialIndex, const aiScene* scene, std::map<int, int>& materialMap, const std::string& directory){
      if(materialMap.contains(materialIndex)){
         return materialMap[materialIndex];
      }else{
         // 只拿每个类型贴图中的第一个贴图
         const aiMaterial* mat = scene->mMaterials[materialIndex];
         aiString diffusePath;
         mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);
         aiString specularPath;
         mat->GetTexture(aiTextureType_SPECULAR, 0, &specularPath);
         // todo 处理 specularPath不存在的情况
         if(specularPath.length == 0){
               materials.emplace_back(
               Texture2D {directory + "/" + diffusePath.C_Str()},
               Texture2D {directory + "/" + diffusePath.C_Str()}
            );
         }else{
            materials.emplace_back(
               Texture2D {directory + "/" + diffusePath.C_Str()},
               Texture2D {directory + "/" + specularPath.C_Str()}
            );
         }
         materialMap[materialIndex] = materials.size() - 1;
         return materials.size() - 1;
      }
   }

   void processMesh(const aiMesh* mesh, const aiScene* scene, std::map<int, int>& materialMap, const std::string& directory){
      // 处理顶点数据
      std::vector<std::array<float, 8>> vertexBufferData;
      vertexBufferData.reserve(mesh->mNumVertices);
      for(int i = 0; i < mesh->mNumVertices; i++){
         std::array<float, 8> data;
         data[0] = mesh->mVertices[i].x;
         data[1] = mesh->mVertices[i].y;
         data[2] = mesh->mVertices[i].z;
         data[3] = mesh->mNormals[i].x;
         data[4] = mesh->mNormals[i].y;
         data[5] = mesh->mNormals[i].z;
         data[6] = mesh->mTextureCoords[0][i].x;
         data[7] = mesh->mTextureCoords[0][i].y;
         vertexBufferData.push_back(data);
      }
      VertexBuffer vbo {vertexBufferData.data(), static_cast<GLsizeiptr>(sizeof(float[8]) * vertexBufferData.size())};
      VertexArray vao;
      vao.addAttribute(vbo, 0, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)0);
      vao.addAttribute(vbo, 1, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));
      vao.addAttribute(vbo, 2, 2, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat)));
      
      // 处理索引数据
      std::vector<unsigned int> indicesData;
      indicesData.reserve(3 * mesh->mNumFaces);
      for(int i = 0; i < mesh->mNumFaces; i++){
         aiFace face = mesh->mFaces[i];
         if(face.mNumIndices != 3){
            throwError("error: every face must have 3 indices");
         }
         for(int j = 0; j < face.mNumIndices; j++){
            indicesData.push_back(face.mIndices[j]);
         }
      }
      ElementBuffer ebo {indicesData.data(), static_cast<GLsizeiptr>(sizeof(unsigned int) * indicesData.size())};

      // 处理材质
      int materialIndex = getMaterialIndex(mesh->mMaterialIndex, scene, materialMap, directory);
      
      meshes.emplace_back(std::move(vbo), std::move(vao), std::move(ebo), *this, materialIndex, 3 * mesh->mNumFaces);
   }
   void processNode(const aiNode* node, const aiScene* scene, std::map<int, int>& meshMap, std::map<int, int>& materialMap, const std::string& directory){
      for(int i = 0; i < node->mNumMeshes; i++){
         int meshIndex = node->mMeshes[i];
         if(meshMap.contains(meshIndex)){
            continue;
         }
         processMesh(scene->mMeshes[meshIndex], scene, materialMap, directory);
         meshMap[meshIndex] = meshes.size() - 1;
      }
      for(int i = 0; i < node->mNumChildren; i++){
         processNode(node->mChildren[i], scene, meshMap, materialMap, directory);
      }
   }
   void buildMeshes(const std::string& path){
      Assimp::Importer importer;
      // aiProcess_Triangulate：将所有图元转换为三角形（如果不是的话）
      // aiProcess_FlipUVs 翻转y轴纹理坐标
      const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

      if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
         throwError(std::string("assimp import model failed:") + importer.GetErrorString());
      }
      const aiNode* node = scene->mRootNode;
      // meshMap 和 materialMap 的key是assimp中的索引，value是Model中的索引
      std::map<int, int> meshMap; 
      std::map<int, int> materialMap;
      std::string directory = path.substr(0, path.find_last_of('/'));
      processNode(node, scene, meshMap, materialMap, directory);
   }
   
public:
   Model(const std::string& path, const glm::mat4& modelTrans = newModel(), float shininess = 64.0f): modelTrans(modelTrans), shininess(shininess){
      try{
         buildMeshes(path);
      }catch(std::string e){
         throwError(fmt::format("import model from {} failed: {}", path, e));
      }
   }

   std::vector<LightObject> lightObjects;
   void addInLightScene(LightScene& lightScene){
      for(auto& mesh: meshes){
         lightObjects.emplace_back(static_cast<LightObjectMeta>(mesh), lightScene);
      }
   }
};

Mesh::operator LightObjectMeta(){
   auto& material = model.materials[materialIndex];
   return {
      vao, material.diffuse, material.specular, model.modelTrans, number, model.shininess,
   };
}
   
int run(){
   try{
      Context ctx {1920, 1080};
      InputProcessor processor;
      Drawer drawer;
      GuiContext guiCtx;
      BasicData basicData {.viewModel = newViewModel(glm::vec3(3.0f, 0.0f, 3.0f))};

      LightContext lightCtx;
      LightScene scene {basicData};
      // 对于nanosuit，贴图是反转的，需要去掉 aiProcess_FlipUVs flag
      Model model {"../model/nanosuit/nanosuit.obj"};
      // Model model {"../model/backpack/backpack.obj"};
      model.addInLightScene(scene);

      DirectionalLightData directionalLightData;
      DirectionalLight directionalLight {directionalLightData, scene};
      DirectionalLightUIController directionalLightController = directionalLightData;

      scene.generateDrawUnits();

      ctx.startLoop([&]{
         GuiFrame frame;
         if(ImGui::Begin("controller")){
            directionalLightController.showControllerPanel();
         }
         ImGui::End();
         drawer.draw([&]{frame.render();});
         processor.processInput();
      });
      
   }catch(std::string e){
      fmt::println("{}", e);
   }
   return 0;
}

} // namespace model

#endif // _MINECPP_MODEL_H_
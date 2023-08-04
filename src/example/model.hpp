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
#include <optional>

#include "../resource.hpp"
#include "../gui.hpp"
#include "../light.hpp"
#include "../transformation.hpp"
#include "../controller.hpp"
#include "../vertex.hpp"
#include "fmt/core.h"


namespace model
{
using namespace minecpp;

class Model;

class Mesh{
private:
   VertexData<true> vertexData;
   Model& model;
   int materialIndex;

public:
   struct LightObjectMetaConstructor{
      VertexArray& vao;
      ElementBuffer& ebo;
      Texture2D& diffuseTexture;
      Texture2D& specularTexture;
      const ObservableValue<glm::mat4>& model;
   };
public:
   Mesh(VertexData<true>&& vertexData, Model& model, int materialIndex)
   :vertexData(std::move(vertexData)), model(model), materialIndex(materialIndex){}
   operator LightObjectMeta();
};

// 禁止移动与拷贝： mesh 含有对 mode 的引用成员，如要移动，则需要将引用变为指针，并且在移动时改变地址
class Model{
friend class Mesh;
private:
   float shininess;
   struct Material{
      Texture2D diffuse;
      std::optional<Texture2D> specular;
   };
   std::vector<Material> materials;
   std::vector<Mesh> meshes;
   ObservableValue<glm::mat4> modelTrans;
   // deleted move semantic
   Model& operator=(Model&&) = delete;
   Model(Model&&) = delete;
   // deleted copy semantic
   Model& operator=(const Model&) = delete;
   Model(const Model&) = delete;
   
   
   
   int getMaterialIndex(int materialIndex, const aiScene* scene, std::map<std::pair<std::string, std::string>, int>& materialMap, const std::string& directory){
      // 只拿每个类型贴图中的第一个贴图
      const aiMaterial* mat = scene->mMaterials[materialIndex];
      aiString diffusePath;
      mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);
      aiString specularPath;
      mat->GetTexture(aiTextureType_SPECULAR, 0, &specularPath);
      std::pair key = {std::string{diffusePath.C_Str()}, std::string{specularPath.C_Str()}};
      if(materialMap.contains(key)){
         return materialMap[key];
      }else{
         materials.emplace_back(
            Texture2D {directory + "/" + diffusePath.C_Str()},
            // 处理 specularPath不存在的情况
            specularPath.length == 0 ? std::nullopt : std::optional(Texture2D {directory + "/" + specularPath.C_Str()})
         );
         materialMap[key] = materials.size() - 1;
         return materials.size() - 1;
      }
   }

   void processMesh(const aiMesh* mesh, const aiScene* scene, std::map<std::pair<std::string, std::string>, int>& materialMap, const std::string& directory){
      // 处理顶点数据
      VertexMeta<true, glm::vec3, glm::vec3, glm::vec2> meta;
      auto& vertexs = meta.vertexs;
      vertexs.reserve(mesh->mNumVertices);
      for(int i = 0; i < mesh->mNumVertices; i++){
         vertexs.push_back({
            glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
            glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
            glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y},
         });
      }
      
      // 处理索引数据
      auto& indices = meta.indices;
      indices.reserve(3 * mesh->mNumFaces);
      fmt::println("num face: {}", mesh->mNumFaces);
      for(int i = 0; i < mesh->mNumFaces; i++){
         aiFace face = mesh->mFaces[i];
         if(face.mNumIndices != 3){
            throwError("error: every face must have 3 indices");
         }
         for(int j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
         }
      }

      // 处理材质
      int materialIndex = getMaterialIndex(mesh->mMaterialIndex, scene, materialMap, directory);

      meshes.emplace_back(createVertexData(meta), *this, materialIndex);
   }
   void processNode(const aiNode* node, const aiScene* scene, std::map<int, int>& meshMap, std::map<std::pair<std::string, std::string>, int>& materialMap, const std::string& directory){
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
      const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate );

      if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
         throwError(std::string("assimp import model failed:") + importer.GetErrorString());
      }
      const aiNode* node = scene->mRootNode;
      // key是assimp中的索引，value是Model中的索引
      std::map<int, int> meshMap; 
      //本来想使用assimp的material的索引作为key，但后来发现assimp中, material不是唯一的，即索引不同却可能指向同一个文件
      // 因此使用文件名为key
      std::map<std::pair<std::string, std::string>, int> materialMap;
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

inline Mesh::operator LightObjectMeta(){
   auto& material = model.materials[materialIndex];
   Texture2D* specularp = nullptr;
   if(material.specular.has_value()){
      specularp = &material.specular.value();
   }
   return {
      vertexData.vao, material.diffuse, specularp, model.modelTrans, model.shininess,
   };
}
   
inline int run(){
   try{
      Context ctx {1920, 1080};
      InputProcessor processor;
      Drawer drawer;
      GuiContext guiCtx;
      BasicData basicData {.viewModel = newViewModel(glm::vec3(3.0f, 0.0f, 3.0f))};

      LightContext lightCtx;
      LightScene scene {basicData};
      // 对于nanosuit，贴图是反转的，需要去掉 aiProcess_FlipUVs flag
      // Model model {"../model/nanosuit/nanosuit.obj"};
      // Model model {"../model/backpack/backpack.obj"};
      Model model {"../model/可莉/可莉.pmx"};
      // Model model {"../model/英招2.0/英招2.0.pmx"};
      // Model model {"../model/英招2.0/武器左.pmx"};
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
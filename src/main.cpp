// #include "example/material.hpp"
// #include "example/3d_lighting_2.hpp"
// #include "example/imgui.hpp"
// #include "example/2d_transform.hpp"
// #include "example/3d_lighting.hpp"
// #include "example/3d_transform.hpp"
// #include "example/3d_transform_template.hpp"
// #include "resource.hpp"
// #include "example/lighting_mappings.hpp"
// #include "example/light_caster.hpp"
#include <iostream>
#include "example/multi_light.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace minecpp;

// void processNode(aiNode *node, const aiScene *scene){
//    // process all the node's meshes (if any)
//    for(int i = 0; i < node->mNumMeshes; i++)
//    {
//       aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
//       meshes.push_back(processMesh(mesh, scene));			
//    }
//    // then do the same for each of its children
//    for(unsigned int i = 0; i < node->mNumChildren; i++)
//    {
//       processNode(node->mChildren[i], scene);
//    }
// }
// Mesh processMesh(aiMesh *mesh, const aiScene *scene)
// {
//    vector<Vertex> vertices;
//    vector<unsigned int> indices;
//    vector<Texture> textures;

//    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
//    {
//       Vertex vertex;
//       // process vertex positions, normals and texture coordinates
//       [...]
//       vertices.push_back(vertex);
//    }
//    // process indices
//    [...]
//    // process material
//    if(mesh->mMaterialIndex >= 0)
//    {
//       [...]
//    }

//    return Mesh(vertices, indices, textures);
// }  



int main()
{
   // max<float>(1.1, 2);
   // return material::run();
   // return lighting_3d_2::run();
   // return imgui::run();
   // return light_caster::run();
   Assimp::Importer importer;
   const aiScene *scene = importer.ReadFile("abc", aiProcess_Triangulate | aiProcess_FlipUVs);
   
   if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
      fmt::println("ERROR from ASSIMP:{}",  importer.GetErrorString());
   }
   // directory = path.substr(0, path.find_last_of('/'));

   // processNode(scene->mRootNode, scene);
   return multi_light::run();
   // return lighting_mappings::run();
}
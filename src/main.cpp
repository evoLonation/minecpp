// #include "example/material.hpp"
// #include "example/3d_lighting_2.hpp"
// #include "example/imgui.hpp"
// #include "example/2d_transform.hpp"
// #include "example/3d_lighting.hpp"
// #include "example/3d_transform.hpp"
// #include "example/3d_transform_template.hpp"
// #include "resource.hpp"
// #include "example/lighting_mappings.hpp"
#include "example/light_caster.hpp"
#include "example/multi_light.hpp"
#include "example/model.hpp"

using namespace minecpp;



// Mesh::operator LightObjectMeta(){
//    return LightObjectMeta{LightObjectMetaConstructor{vao, ebo, model.materials[materialIndex].diffuse, model.materials[materialIndex].specular, model.modelTrans, model.shininess}};
// }

int main()
{
   // max<float>(1.1, 2);
   // return material::run();
   // return lighting_3d_2::run();
   // return imgui::run();
   return light_caster::run();
   
   // directory = path.substr(0, path.find_last_of('/'));

   // processNode(scene->mRootNode, scene);
   return multi_light::run();
   return model::run();
   // return lighting_mappings::run();
}
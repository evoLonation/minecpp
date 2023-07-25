#ifndef _MINECPP_MULTI_LIGHT_H_
#define _MINECPP_MULTI_LIGHT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../resource.hpp"
#include "../gui.hpp"
#include "../light.hpp"
#include "../transformation.hpp"
#include "../controller.hpp"


namespace multi_light
{
using namespace minecpp;
   
float vertices[] = {
   // positions          // normals           // texture coords
   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

struct ObjectInfo {
   static VertexArray* vao;
   static Texture2D* diffuse;
   static Texture2D* specular;
   ChangeableObservable<glm::mat4> model;
   float shininess;

   // ObjectInfo& operator=(ObjectInfo&& obj) = delete;
   // ObjectInfo(ObjectInfo&& obj) noexcept = default;
   // ObjectInfo& operator=(const ObjectInfo& obj) = delete;
   // ObjectInfo(const ObjectInfo& obj) = delete;

   // struct LightObjectMetaConstructor{
   //    VertexArray& vao;
   //    Texture2D& diffuseTexture;
   //    Texture2D& specularTexture;
   //    const ObservableValue<glm::mat4>& model;
   //    const int number;
   //    const float& shininess;
   // };
   // operator LightObjectMeta(){
   //    return LightObjectMeta{LightObjectMetaConstructor{*vao, *diffuse, *specular, model, 64, shininess}};
   // }
   ObjectInfo(const glm::mat4& model = newModel(), float shininess = 64.0f): model(model), shininess(shininess){}
   operator LightObjectMeta(){
      return {*vao, *diffuse, *specular, this->model, 64, shininess};
   }
};

VertexArray* ObjectInfo::vao;
Texture2D* ObjectInfo::diffuse;
Texture2D* ObjectInfo::specular;

class ObjectUIController: UnCopyMoveable{
private:
   ObjectInfo* object;
   ChangeableObservable<glm::vec3> scale;
   ChangeableObservable<glm::vec3> position;
   ChangeableObservable<glm::vec3> axios;
   ChangeableObservable<float> angle;
   ModelController controller;
   ReactiveObserver<glm::vec3, glm::vec3, glm::vec3, float> modelChanger;
public:
   ObjectUIController(ObjectInfo& object): ObjectUIController(&object){}
   ObjectUIController(): ObjectUIController(nullptr){}
   ObjectUIController(ObjectInfo* object): object(object), 
   position([this]{ if(this->object != nullptr) return ModelComputer::computePosition(this->object->model.get()); else return glm::vec3(0.0f);}()),
   scale([this]{ if(this->object != nullptr) return ModelComputer::computeScale(this->object->model.get()); else return glm::vec3(0.0f);}()),
   axios({1.0f, 0.0f, 0.0f}), controller([this]{return this->object != nullptr ? &this->object->model : nullptr;}()),  
   modelChanger([this](const glm::vec3& scale, const glm::vec3& position, const glm::vec3& axios, float angle){
      // *this->object->model = newModel(position, scale);
      *this->object->model = newModel(position);

      // this->controller.isSelf = false;
      // this->controller.translate(position);
      this->controller.isSelf = true;
      this->controller.rotate(angle, axios);
      this->object->model = *this->object->model * glm::scale(scale);
   }, scale, position, axios, angle)
   {}
   ObjectUIController& operator=(ObjectInfo& object){
      bool newObject = false;
      if(this->object != &object){
         newObject = true;
      }
      this->object = &object;
      controller = object.model;
      if(newObject){
         position = ModelComputer::computePosition(this->object->model.get());
         scale = ModelComputer::computeScale(this->object->model.get());
         axios = {1.0f, 0.0f, 0.0f};
         angle = 0.0f;
      }
      
      return *this;
   }

   void showControllerPanel(){
      if(object == nullptr){
         ImGui::Text("not yet bound to any objects");
         return;
      }
      slider("object scale", scale, glm::vec3{0.0f}, glm::vec3{5.0f});
      slider("object position", position);
      slider("object axios", axios);
      slider("object angle", angle, 0.0f, 360.f);
      slider("object shininess", object->shininess, 0.0f, 360.f);
   }
};



int run(){
   try{
      Context ctx {1920, 1080};
      InputProcessor processor;
      Drawer drawer;
      GuiContext guiCtx;

      VertexBuffer vbo {vertices, sizeof(vertices)};
      VertexArray vao;
      vao.addAttribute(vbo, 0, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)0);
      vao.addAttribute(vbo, 1, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));
      vao.addAttribute(vbo, 2, 2, GL_FLOAT, false, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat)));

      ObjectInfo::vao = &vao;

      Texture2D texture {"../image/container2.png"};
      Texture2D specular {"../image/container2_specular.png"};

      ObjectInfo::diffuse = &texture;
      ObjectInfo::specular = &specular;

      LightContext lightContext;

      BasicData basicData{
         .viewModel = newViewModel(glm::vec3(3.0f, 0.0f, 3.0f)),
      };

      LightScene lightScene {basicData};

      // DirectionalLightData directionalLight {
      //    .color = glm::vec3(1.0f, 1.0f, 1.0f),
      //    .direction = glm::vec3(0.0f, -1.0f, 0.0f),
      //    .lightScene = lightScene
      // };

      DirectionalLightData directionalLightData;
      DirectionalLight directionalLight {directionalLightData, lightScene};
      std::vector<ObjectInfo> objectDatas;
      std::vector<LightObject> objects;
      std::vector<PointLightData> pointLightDatas;
      std::vector<PointLight> pointLights;
      std::vector<SpotLightData> spotLightDatas;
      std::vector<SpotLight> spotLights;

      DirectionalLightUIController directionalLightController = directionalLightData;
      ObjectUIController modelController;
      SpotLightUIController spotLightController;
      PointLightUIController pointLightController;

      auto dataChangeHandler = [&](){
         objects.clear();
         for(auto& data: objectDatas){
            objects.emplace_back(data, lightScene);
         }
         pointLights.clear();
         for(auto& data: pointLightDatas){
            pointLights.emplace_back(data, lightScene);
         }
         spotLights.clear();
         for(auto& data: spotLightDatas){
            spotLights.emplace_back(data, lightScene);
         }
         lightScene.generateDrawUnits();
      };
      
      objectDatas.emplace_back();
      dataChangeHandler();


      bool creation[3] = {0};
      int controller = 0;
      int objectSelect = 0;
      int pointLightSelect = 0;
      int spotLightSelect = 0;


      ctx.startLoop([&]{
         GuiFrame frame;
         ImGui::ShowDemoWindow();
         
         // begin 开启一个新的窗口
         // 当窗口被折叠时返回false
         // 但是不管返回什么，都需要执行end
         if(ImGui::Begin("controller")){
            ImGui::SeparatorText("creation window choose");
            if(ImGui::BeginTable("creation window choose", 3)){
               ImGui::TableNextColumn();
               ImGui::Checkbox("cube object", &creation[0]);
               ImGui::TableNextColumn();
               ImGui::Checkbox("point light", &creation[1]);
               ImGui::TableNextColumn();
               ImGui::Checkbox(" spot light", &creation[2]);
               ImGui::EndTable();
            }
            ImGui::SeparatorText("controller choose");
            if(ImGui::BeginTable("controller choose", 2)){
               ImGui::TableNextColumn();
               ImGui::RadioButton("cube object", &controller, 0);
               ImGui::TableNextColumn();
               ImGui::RadioButton("directional light", &controller, 1);
               ImGui::TableNextColumn();
               ImGui::RadioButton("point light", &controller, 2);
               ImGui::TableNextColumn();
               ImGui::RadioButton(" spot light", &controller, 3);
               ImGui::EndTable();
            }
            if(controller == 0){
               ImGui::SeparatorText("cube object controller");
               std::map<int, std::string> popupElements;
               for(int i = 0; i < objectDatas.size(); i++){
                  popupElements[i] = fmt::format("object {}", i+1);
               }
               showPopup(objectSelect, popupElements);
               modelController = objectDatas[objectSelect];
               modelController.showControllerPanel();
            }else if(controller == 1){
               ImGui::SeparatorText("directional light controller");
               directionalLightController.showControllerPanel();
            }else if(controller == 2){
               ImGui::SeparatorText("point light controller");
               std::map<int, std::string> popupElements;
               for(int i = 0; i < pointLightDatas.size(); i++){
                  popupElements[i] = fmt::format("point light {}", i+1);
               }
               showPopup(pointLightSelect, popupElements);
               pointLightController = pointLightDatas[pointLightSelect];
               pointLightController.showControllerPanel();
            }else if(controller == 3){
               ImGui::SeparatorText("spot light controller");
               std::map<int, std::string> popupElements;
               for(int i = 0; i < spotLightDatas.size(); i++){
                  popupElements[i] = fmt::format("point light {}", i+1);
               }
               showPopup(spotLightSelect, popupElements);
               spotLightController = spotLightDatas[spotLightSelect];
               spotLightController.showControllerPanel();
            }
         }
         ImGui::End();
         
         if(creation[0]){
            if(ImGui::Begin("cube object creation", &creation[0])){
               static glm::vec3 position;
               slider("position", position);
               if(ImGui::Button("create new object")){
                  objectDatas.emplace_back(newModel(position));
                  dataChangeHandler();
               }
            }
            ImGui::End();
         }
         
         if(creation[1]){
            if(ImGui::Begin("point light creation", &creation[1])){
               static glm::vec3 position;
               static glm::vec3 color;
               static float distance;
               slider("position", position);
               ImGui::ColorEdit3("color", glm::value_ptr(color));
               slider("distance", distance, 0.0f, 100.0f);
               if(ImGui::Button("create new point light")){
                  pointLightDatas.emplace_back(color, position, distance);
                  dataChangeHandler();
               }
            }
            ImGui::End();
         }
         if(creation[2]){
            if(ImGui::Begin(" spot lignt creation", &creation[2])){
               static glm::vec3 position;
               static glm::vec3 direction;
               static glm::vec3 color;
               static float distance;
               static float inner;
               static float outer;
               slider("position", position);
               slider("direction", direction);
               ImGui::ColorEdit3("color", glm::value_ptr(color));
               slider("distance", distance, 0.0f, 100.0f);
               slider("inner cut off", inner, 0.0f, 90.0f);
               slider("outer cut off", outer, 0.0f, 90.0f);
               if(ImGui::Button("create new spot light")){
                  spotLightDatas.emplace_back(color, position, direction, distance, inner, outer);
                  dataChangeHandler();
               }
            }
            ImGui::End();
         }

         

         drawer.draw([&]{frame.render();});
         processor.processInput();
      });

   }catch(std::string e){
      fmt::println("{}", e);
   }
   return 0;
}

} // namespace multi_light

#endif // _MINECPP_MULTI_LIGHT_H_
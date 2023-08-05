#ifndef _MINECPP_MULTI_LIGHT_H_
#define _MINECPP_MULTI_LIGHT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <optional>

#include "../resource.hpp"
#include "../gui.hpp"
#include "../light.hpp"
#include "../transformation.hpp"
#include "../controller.hpp"
#include "../vertex.hpp"
#include "../input.hpp"


namespace multi_light
{
using namespace minecpp;
   
inline std::array<std::tuple<glm::vec3, glm::vec3, glm::vec2>, 36> vertices = {
   // positions          // normals           // texture coords
   std::tuple{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  0.0f}},

   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  0.0f}},

   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},

   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},

   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},

   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}},
   {glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}}
};

struct ObjectInfo {
   static VertexArray* vao;
   static Texture2D* diffuse;
   static Texture2D* specular;
   ObservableValue<glm::mat4> model;
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
      return {*vao, *diffuse, specular, this->model, shininess};
   }
};

inline VertexArray* ObjectInfo::vao;
inline Texture2D* ObjectInfo::diffuse;
inline Texture2D* ObjectInfo::specular;

class ObjectUIController: private ObservableValue<ObjectInfo*>, public AbstractObserver<ObjectInfo*const>{
private:
   using Object = ObservableValue<ObjectInfo*>;
   ObservableValue<glm::vec3> scale;
   ObservableValue<glm::vec3> position;
   ObservableValue<glm::vec3> axios;
   ObservableValue<float> angle;
   
   std::optional<ReactiveBinder<glm::mat4, glm::vec3, glm::vec3, glm::vec3, float>> modelChanger;
   void handle(ObjectInfo* const & object) override {
      if(object == nullptr){
         modelChanger.reset();
      }else{
         position = ModelComputer::computePosition(object->model.get());
         scale = ModelComputer::computeScale(object->model.get());
         auto oldModel = object->model.get();
         modelChanger.emplace([this](const glm::vec3& scale, const glm::vec3& position, const glm::vec3& axios, float angle){
            // *this->object->model = newModel(position, scale);
            auto model {newModel(position) * glm::rotate(glm::mat4(1.0f), glm::radians(angle), axios) * glm::scale(scale)} ;
            // this->controller.isSelf = false;
            // this->controller.translate(position);
            // controller.isSelf = true;
            // controller.rotate(angle, axios);
            // model = *model * glm::scale(scale);
            return model;
         }, object->model, scale, position, axios, angle);
         object->model = oldModel;
      }
   }
public:
   ObjectUIController(ObjectInfo& object): ObjectUIController(&object){}

   ObjectUIController(): ObjectUIController(nullptr){}
   
   ObjectUIController(ObjectInfo* object):
      ObservableValue<ObjectInfo *>(object), 
      AbstractObserver<ObjectInfo*const>(static_cast<Object&>(*this)),
      axios({1.0f, 0.0f, 0.0f}) {}
   
   ObjectUIController& operator=(ObjectInfo& object){
      Object::operator=(&object);
      return *this;
   }

   void reset(){
      Object::operator=(nullptr);
   }

   void showControllerPanel(){
      if(Object::get() == nullptr){
         ImGui::Text("not yet bound to any objects");
         return;
      }
      slider("object scale", scale, glm::vec3{0.0f}, glm::vec3{5.0f});
      slider("object position", position);
      slider("object axios", axios);
      slider("object angle", angle, 0.0f, 360.f);
      slider("object shininess", Object::get()->shininess, 0.0f, 360.f);
   }

   // 如果实现，需要重新构造 AbstractObserver 
   // deleted move semantic
   ObjectUIController& operator=(ObjectUIController&&) = delete;
   ObjectUIController(ObjectUIController&&) = delete;
   // deleted copy semantic
   ObjectUIController& operator=(const ObjectUIController&) = delete;
   ObjectUIController(const ObjectUIController&) = delete;
   
};



inline int run(){
   try{
      Context ctx {1920, 1080};
      InputProcessor processor;
      Drawer drawer;
      GuiContext guiCtx;

      VertexMeta<false, glm::vec3, glm::vec3, glm::vec2> meta;
      for(auto& vertex: vertices){
         meta.vertexs.push_back(vertex);
      }
      VertexData vertexData {createVertexData(meta)};

      ObjectInfo::vao = &vertexData.vao;

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

      auto objectChangeHandler = [&] (auto&& changer) {
         modelController.reset();
         objects.clear();
         changer();
         for(auto& data: objectDatas){
            objects.emplace_back(data, lightScene);
         }
         lightScene.generateDrawUnits();
      };
      auto pointLightChangeHandler = [&] (auto&& changer) {
         pointLightController.reset();
         pointLights.clear();
         changer();
         for(auto& data: pointLightDatas){
            pointLights.emplace_back(data, lightScene);
         }
         lightScene.generateDrawUnits();
      };
      auto spotLightChangeHandler = [&] (auto&& changer) {
         spotLightController.reset();
         spotLights.clear();
         changer();
         for(auto& data: spotLightDatas){
            spotLights.emplace_back(data, lightScene);
         }
         lightScene.generateDrawUnits();
      };

      // objectDatas.emplace_back();
      objectChangeHandler([&]{objectDatas.emplace_back();});
      
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
                  // 在 更新 objectDatas 容器之前，需要先解除之前的所有对这些对象的引用
                  objectChangeHandler([&]{
                     objectDatas.emplace_back(newModel(position));
                  });
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
                  pointLightChangeHandler([&]{
                     pointLightDatas.emplace_back(color, position, distance);
                  });
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
                  spotLightChangeHandler([&]{
                     spotLightDatas.emplace_back(color, position, direction, distance, inner, outer);
                  });
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
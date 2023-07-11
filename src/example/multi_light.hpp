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
#include "../model.hpp"


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

struct CubeInfo {
   static VertexArray* vao;
   static Texture2D* diffuse;
   static Texture2D* specular;

   ChangeableObservable<glm::mat4> model;
   float shininess;

   struct LightObjectMetaConstructor{
      VertexArray& vao;
      Texture2D& diffuseTexture;
      Texture2D& specularTexture;
      const ObservableValue<glm::mat4>& model;
      const int number;
      const float& shininess;
   };
   operator LightObjectMeta(){
      return LightObjectMeta{LightObjectMetaConstructor{*vao, *diffuse, *specular, model, 64, shininess}};
   }
   CubeInfo(const glm::mat4& model = newModel(), float shininess = 64.0f): model(model), shininess(shininess){}
};

VertexArray* CubeInfo::vao;
Texture2D* CubeInfo::diffuse;
Texture2D* CubeInfo::specular;

class ModelUIController: UnCopyMoveable{
private:
   ChangeableObservable<glm::mat4>* model;
   ChangeableObservable<glm::vec3> position;
   ChangeableObservable<glm::vec3> axios;
   ChangeableObservable<float> angle;
   ModelController controller;
   ReactiveObserver<glm::vec3, glm::vec3, float> modelChanger;
public:
   ModelUIController(ChangeableObservable<glm::mat4>& model): model(&model), axios({1.0f, 0.0f, 0.0f}), controller(*this->model),  
   modelChanger([this](const glm::vec3& position, const glm::vec3& axios, float angle){
      *this->model = newModel();
      this->controller.isSelf = false;
      this->controller.translate(position);
      this->controller.isSelf = true;
      this->controller.rotate(angle, axios);
   }, position, axios, angle)
   {}
   ModelUIController& operator=(ChangeableObservable<glm::mat4>& model){
      this->model = &model;
      return *this;
   }

   void showControllerPanel(){
      ImGui::SliderFloat("cube position: x", &position->x, -5.0f, 5.0f);
      ImGui::SliderFloat("cube position: y", &position->y, -5.0f, 5.0f);
      ImGui::SliderFloat("cube position: z", &position->z, -5.0f, 5.0f);
      ImGui::SliderFloat("cube axios: x", &axios->x, -5.0f, 5.0f);
      ImGui::SliderFloat("cube axios: y", &axios->y, -5.0f, 5.0f);
      ImGui::SliderFloat("cube axios: z", &axios->z, -5.0f, 5.0f);
      ImGui::SliderFloat("cube angle: ", &angle.val(), 0.0f, 360.0f);
      position.mayNotice();
      axios.mayNotice();
      angle.mayNotice();
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

      CubeInfo::vao = &vao;

      Texture2D texture {"../image/container2.png"};
      Texture2D specular {"../image/container2_specular.png"};

      CubeInfo::diffuse = &texture;
      CubeInfo::specular = &specular;

      // view, view pos
      ChangeableObservable viewModel {newViewModel(glm::vec3(3.0f, 0.0f, 3.0f))};
      auto viewPos = makeReactiveValue([](const glm::mat4& viewModel){
         return ModelComputer::computeViewPosition(viewModel);
      }, viewModel);
      ModelMoveSetter modelSetter { viewModel };
      // projection
      ProjectionCoord projectionCoord;

      DirectionalLightExample directionalLight;
      directionalLight.direction = glm::vec3(0.0f, -1.0f, 0.0f);
      directionalLight.color = glm::vec3(1.0f, 1.0f, 1.0f);

      LightScene lightScene {projectionCoord.getProjection(), viewModel, directionalLight};

      std::vector<CubeInfo> objects;
      objects.emplace_back();
      objects.emplace_back(newModel(glm::vec3{1.0f, 1.0f, 1.0f}));
      ModelUIController modelController {objects[0].model};
      lightScene.setLightObjects(objects.begin(), objects.end());


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
               modelController.showControllerPanel();
            }else if(controller == 1){
               ImGui::SeparatorText("directional light controller");
               std::map<int, std::string> popupElements;
               for(int i = 0; i < objects.size(); i++){
                  popupElements[i] = fmt::format("object {}", i+1);
               }
               showPopup(objectSelect, popupElements);
               
            }else if(controller == 2){
               ImGui::SeparatorText("point light controller");
            }else if(controller == 3){
               ImGui::SeparatorText("spot light controller");
            }
         }
         ImGui::End();
         
         if(creation[0]){
            if(ImGui::Begin("cube object creation", &creation[0])){

            }
            ImGui::End();
         }
         
         if(creation[1]){
            if(ImGui::Begin("point light creation", &creation[1])){
            
            }
            ImGui::End();
         }
         if(creation[2]){
            if(ImGui::Begin(" spot lignt creation", &creation[2])){
            
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
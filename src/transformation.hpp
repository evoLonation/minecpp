#ifndef _MINECPP_TRANSFORMATION_H_
#define _MINECPP_TRANSFORMATION_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "glm/fwd.hpp"
#include "tool.hpp"
#include "resource.hpp"
#include "input.hpp"

namespace minecpp
{
   
inline glm::mat4 newViewModel(const glm::vec3& position, const glm::vec3& target = glm::vec3(0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)){
   // 从相机的方向上看过去，其右边是x轴正方向，上边是y轴正方向，前边是z轴反方向
   // 使用glm::lookAt方法，给定相机位置、目标（相机对准的地方，z轴反方向）与y轴方向（都是相对于世界坐标系），得到相机的view矩阵
   // 参数无需单位化
   return glm::lookAt(position, target - position, up);
}
inline glm::mat4 newModel(const glm::vec3& location, const glm::vec3& scale){
   return glm::translate(glm::mat4(1.0f), location) * glm::scale(scale);
}
inline glm::mat4 newModel(const glm::vec3& location = glm::vec3(), float scale = 1.0f){
   return newModel(location, glm::vec3(scale, scale, scale));
}


class ModelController{
private:
   ObservableValue<glm::mat4>* model;
public:
   bool isSelf;

   ModelController(ObservableValue<glm::mat4>* model, bool isSelf = false): model(model), isSelf(isSelf){}
   ModelController(bool isSelf = false): ModelController(nullptr, isSelf){}
   // model必须是空间矩阵，否则是未定义行为
   ModelController(ObservableValue<glm::mat4>& model, bool isSelf = false): ModelController(&model, isSelf){}
   ModelController& operator=(ObservableValue<glm::mat4>* model){
      this->model = model;
      return *this;
   }
   ModelController& operator=(ObservableValue<glm::mat4>& model){
      return this->operator=(&model);
   }
   void translate(const glm::vec3& vec){
      if(isSelf){
         **model = model->get() * glm::translate(vec);
         model->mayNotify();
      }else{
         // 空间矩阵相对于目标坐标系进行平移
         **model = glm::translate(vec) * model->get();
         model->mayNotify();
      }
   }
   void translateX(float delta){
      translate({delta, 0.0f, 0.0f});
   }
   void translateY(float delta){
      translate({0.0f, delta, 0.0f});
   }
   void translateZ(float delta){
      translate({0.0f, 0.0f, delta});
   }
   // 可以不是 normalize 的
   void rotate(float angle, const glm::vec3& axios){
      if(isSelf){
         // 空间矩阵相对于自己进行旋转
         **model = model->get() * glm::mat4_cast(glm::angleAxis(glm::radians(angle), glm::normalize(axios)));
         // 这种方式效果也是一样的
         // **model = glm::rotate(model->get(), glm::radians(angle), axios);
         model->mayNotify();
      }else{
         // 空间矩阵相对于目标坐标系进行旋转
         **model = glm::mat4_cast(glm::angleAxis(glm::radians(angle), glm::normalize(axios))) * model->get();
         model->mayNotify();
      }
   }
   void rotateX(float angle){
      rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
   }
   void rotateY(float angle){
      rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
   }
   void rotateZ(float angle){
      rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
   }
};

class ModelComputer{
public:
   static glm::vec3 computePosition(const glm::mat4& model){
      return glm::vec3(model[3]);
   }
   static glm::vec3 computeScale(const glm::mat4& model){
      return glm::vec3 {
         glm::length(glm::vec3(model[0])),
         glm::length(glm::vec3(model[1])),
         glm::length(glm::vec3(model[2]))
      };
   }
   static glm::vec3 computeViewPosition(const glm::mat4& viewModel){
      return glm::vec3(glm::inverse(viewModel)[3]);
   }
   static glm::mat3 computeNormalModel(const glm::mat4& model){
      return glm::mat3(glm::transpose(glm::inverse(model)));
   }
};

class ProjectionMeta{
public:
   ObservableValue<float> fovy;
   ObservableValue<float> near;
   ObservableValue<float> far;
};

class ProjectionCoord: public ProjectionMeta{
public:
   using ReactiveProjection = ReactiveValue<glm::mat4, int, int, float, float, float>;
   ReactiveProjection projection;
private:
   ReactiveProjection getReactiveProjection(){
      return {
         [](auto width, auto height, auto fovy, auto near, auto far){
            return glm::perspective(glm::radians(fovy), width * 1.0f / height , near, far);
         }, 
         Context::getInstance().getWidth(), Context::getInstance().getHeight(), this->fovy, this->near, this->far
      };
   }
public:
   ProjectionCoord(float fovy = 45.0f, float near = 0.1f, float far = 100.0f):
      ProjectionMeta{.fovy = fovy, .near = near, .far = far},
      projection(getReactiveProjection()){}

   // copy semantic
   ProjectionCoord& operator=(const ProjectionCoord& projectionCoord) {
      // 只需要把元数据拿过来，reactive不变
      ProjectionMeta::operator=(projectionCoord);
      // projection = getReactiveProjection();
      return *this;
   }
   ProjectionCoord(const ProjectionCoord& projectionCoord) :
      ProjectionMeta(projectionCoord),
      projection (getReactiveProjection()){}
   
   // move semantic
   ProjectionCoord& operator=(ProjectionCoord&& projectionCoord) {
      // 只需要把元数据拿过来，reactive不变
      ProjectionMeta::operator=(std::move(projectionCoord));
      // projection = getReactiveProjection();
      return *this;
   }
   ProjectionCoord(ProjectionCoord&& projectionCoord) :
      ProjectionMeta (std::move(projectionCoord)),
      projection (getReactiveProjection()){}

};

class ModelMoveSetter{
private:
   std::vector<KeyHoldHandler> handlerSetters;
   ModelController viewModelController;
   void createSetters(){
      auto add = [this](int key, const std::function<void()>& handler){
         this->handlerSetters.emplace_back(key, [handler](auto _){handler();});
      };
      add(GLFW_KEY_A, [this](){this->viewModelController.translateX(moveSpeed);});
      add(GLFW_KEY_D, [this](){this->viewModelController.translateX(-moveSpeed);});
      add(GLFW_KEY_Z, [this](){this->viewModelController.translateY(-moveSpeed);});
      add(GLFW_KEY_X, [this](){this->viewModelController.translateY(moveSpeed);});
      add(GLFW_KEY_W, [this](){this->viewModelController.translateZ(moveSpeed);});
      add(GLFW_KEY_S, [this](){this->viewModelController.translateZ(-moveSpeed);});
      add(GLFW_KEY_L, [this](){this->viewModelController.rotateY(rotateSpeed);});
      add(GLFW_KEY_J, [this](){this->viewModelController.rotateY(-rotateSpeed);});
      add(GLFW_KEY_I, [this](){this->viewModelController.rotateX(-rotateSpeed);});
      add(GLFW_KEY_K, [this](){this->viewModelController.rotateX(rotateSpeed);});
      add(GLFW_KEY_U, [this](){this->viewModelController.rotateZ(-rotateSpeed);});
      add(GLFW_KEY_O, [this](){this->viewModelController.rotateZ(rotateSpeed);});
   }
public:
   float moveSpeed = 0.05f;
   float rotateSpeed = 1.0f;
   ModelMoveSetter(ObservableValue<glm::mat4>& viewModel, bool isSelf = false):viewModelController(viewModel, isSelf){
      createSetters();
   }
   void isSelf(bool isSelf){
      viewModelController.isSelf = isSelf;
   }
   ModelMoveSetter& operator=(ModelMoveSetter&& setter){
      viewModelController = std::move(setter.viewModelController);
      handlerSetters.clear();
      createSetters();
      return *this;
   }
   ModelMoveSetter(ModelMoveSetter&& setter): viewModelController(std::move(setter.viewModelController)) {
      handlerSetters.clear();
      createSetters();
   }
   // deleted copy semantic
   ModelMoveSetter& operator=(const ModelMoveSetter&) = delete;
   ModelMoveSetter(const ModelMoveSetter&) = delete;
   
};


} // namespace minecpp
#endif // _MINECPP_TRANSFORMATION_H_
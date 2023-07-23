#ifndef _MINECPP_MODEL_H_
#define _MINECPP_MODEL_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "tool.hpp"
#include "resource.hpp"

namespace minecpp
{
   
glm::mat4 newViewModel(const glm::vec3& position, const glm::vec3& target = glm::vec3(0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)){
   // 从相机的方向上看过去，其右边是x轴正方向，上边是y轴正方向，前边是z轴反方向
   // 使用glm::lookAt方法，给定相机位置、目标（相机对准的地方，z轴反方向）与y轴方向（都是相对于世界坐标系），得到相机的view矩阵
   // 参数无需单位化
   return glm::lookAt(position, target - position, up);
}
glm::mat4 newModel(const glm::vec3& location, const glm::vec3& scale){
   return glm::translate(glm::mat4(1.0f), location) * glm::scale(scale);
}
glm::mat4 newModel(const glm::vec3& location = glm::vec3(), float scale = 1.0f){
   return newModel(location, glm::vec3(scale, scale, scale));
}


class ModelController{
private:
   ChangeableObservable<glm::mat4>* model;
public:
   bool isSelf;

   ModelController(ChangeableObservable<glm::mat4>* model, bool isSelf = false): model(model), isSelf(isSelf){}
   ModelController(bool isSelf = false): ModelController(nullptr, isSelf){}
   // model必须是空间矩阵，否则是未定义行为
   ModelController(ChangeableObservable<glm::mat4>& model, bool isSelf = false): ModelController(&model, isSelf){}
   ModelController& operator=(ChangeableObservable<glm::mat4>* model){
      this->model = model;
      return *this;
   }
   ModelController& operator=(ChangeableObservable<glm::mat4>& model){
      return this->operator=(&model);
   }
   void translate(const glm::vec3& vec){
      if(isSelf){
         **model = model->get() * glm::translate(vec);
         model->mayNotice();
      }else{
         // 空间矩阵相对于目标坐标系进行平移
         **model = glm::translate(vec) * model->get();
         model->mayNotice();
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
         model->mayNotice();
      }else{
         // 空间矩阵相对于目标坐标系进行旋转
         **model = glm::mat4_cast(glm::angleAxis(glm::radians(angle), glm::normalize(axios))) * model->get();
         model->mayNotice();
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

// 该类的含有的资源包括“对其他资源的设置”，因此不能轻易拷贝和移动
class ProjectionCoord: public UnCopyMoveable{
private:
   glm::mat4 projection;
public:
   ProjectionCoord(float fovy = 45.0f, float near = 0.1f, float far = 100.0f){
      Context& ctx = Context::getInstance(); 
      auto& width = ctx.getWidth();
      auto& height = ctx.getHeight();
      auto computePj = [fovy, near, far, this](){
         Context& ctx = Context::getInstance(); 
         auto width = ctx.getWidth().get();
         auto height = ctx.getHeight().get();
         this->projection = glm::perspective(glm::radians(fovy), width * 1.0f / height , near, far);
      };
      computePj();
      width.addObserver(computePj);
      height.addObserver(computePj);
   }

   const glm::mat4& getProjection() const {
      return projection;
   }
};

class ModelMoveSetter: public UnCopyable{
private:
   std::vector<KeyHoldHandlerSetter> handlerSetters;
   ModelController viewModelController;
public:
   float moveSpeed = 0.05f;
   float rotateSpeed = 1.0f;
   ModelMoveSetter(ChangeableObservable<glm::mat4>& viewModel, bool isSelf = false):viewModelController(viewModel, isSelf){
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
   void isSelf(bool isSelf){
      viewModelController.isSelf = isSelf;
   }
   ModelMoveSetter& operator=(ModelMoveSetter&& setter){
      viewModelController = std::move(setter.viewModelController);
      return *this;
   }
   ModelMoveSetter(ModelMoveSetter&& setter): viewModelController(std::move(setter.viewModelController)) {}
};


} // namespace minecpp



#endif // _MINECPP_MODEL_H_
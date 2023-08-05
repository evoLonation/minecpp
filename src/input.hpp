#ifndef _MINECPP_INPUT_H_
#define _MINECPP_INPUT_H_
#include "tool.hpp"
#include "resource.hpp"
#include <chrono>

namespace minecpp {

/*****************************************************/
/*****************************************************/
/****************** INPUTPROCESSOR *******************/
/*****************************************************/
/*****************************************************/

class KeyDownHandler: public AutoLoader<KeyDownHandler>{
private:
    std::function<void()> handler;
    RefContainer<KeyDownHandler>& getContainer(int key);
public:
    void operator()(){
        handler();
    }
public:
    KeyDownHandler(int key, auto&& callable): AutoLoader<KeyDownHandler>(getContainer(key)), handler(std::forward<decltype(callable)>(callable)){}
};

class KeyHoldHandler: public AutoLoader<KeyHoldHandler>{
private:
    std::function<void(int holdMilli)> handler;
    RefContainer<KeyHoldHandler>& getContainer(int key);
public:
    void operator()(int holdMilli){
        handler(holdMilli);
    }
public:
    KeyHoldHandler(int key, auto&& callable): AutoLoader<KeyHoldHandler>(getContainer(key)), handler(std::forward<decltype(callable)>(callable)){}
};

class KeyReleaseHandler: public AutoLoader<KeyReleaseHandler>{
private:
    std::function<void(int holdMilli)> handler;
    RefContainer<KeyReleaseHandler>& getContainer(int key);
public:
    void operator()(int holdMilli){
        handler(holdMilli);
    }
public:
    KeyReleaseHandler(int key, auto&& callable): AutoLoader<KeyReleaseHandler>(getContainer(key)), handler(std::forward<decltype(callable)>(callable)){}
};

class InputProcessor: public ProactiveSingleton<InputProcessor>{
    friend KeyDownHandler;
    friend KeyHoldHandler;
    friend KeyReleaseHandler;
private:

   std::map<int, std::chrono::_V2::system_clock::time_point> pressedKeys;
   std::map<int, RefContainer<KeyDownHandler>> keyDownHandlers;
   std::map<int, RefContainer<KeyReleaseHandler>> keyReleaseHandlers;
   std::map<int, RefContainer<KeyHoldHandler>> keyHoldHandlers;
   
public:
   InputProcessor(){
      // 须在创建窗口后、开始渲染前注册回调函数
      glfwSetKeyCallback(Context::getInstance().getWindow(), [](GLFWwindow *window, int key, int scancode, int action, int mods){
         auto& processor = InputProcessor::getInstance();
         if(action == GLFW_PRESS){
            if(processor.keyDownHandlers.contains(key)){
               for(auto& handler : processor.keyDownHandlers[key]){
                  handler();
               }
            }
            auto startTime = std::chrono::high_resolution_clock::now();
            processor.pressedKeys.insert({key, startTime});
         }else if(action == GLFW_RELEASE){
            // if(processor.keyUpHandlers.contains(key)){
            //    for(const auto& handler : processor.keyUpHandlers[key]){
            //       handler();
            //    }
            // }
            if(processor.keyReleaseHandlers.contains(key)){
               auto startTime = processor.pressedKeys[key];
               auto endTime = std::chrono::high_resolution_clock::now();
               auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
               for(auto& handler : processor.keyReleaseHandlers[key]){
                  handler(duration.count());
               }
            }
            processor.pressedKeys.erase(key);
         }

      });
   };

private:
   std::chrono::high_resolution_clock::time_point lastTime {std::chrono::high_resolution_clock::now()};
public:
   void processInput(){
      glfwPollEvents();

      auto _nowTime = std::chrono::high_resolution_clock::now();
      auto _duration = std::chrono::duration_cast<std::chrono::milliseconds>(_nowTime - lastTime);
      if(_duration.count() < 16.7){
         return;
      }else{
         lastTime = std::chrono::high_resolution_clock::now();
      }

      bool hasAny = false;
      for(const auto& pair: pressedKeys){
         auto _key = pair.first;
         std::chrono::_V2::system_clock::time_point endTime;
         if(keyHoldHandlers.contains(_key)){
            if(!hasAny){
               endTime = std::chrono::high_resolution_clock::now();
               hasAny = true;
            }
            auto startTime = pair.second;
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            for(auto& handler : keyHoldHandlers[_key]){
               handler(duration.count());
            }
         }
      }
   }
};

inline RefContainer<KeyDownHandler>& KeyDownHandler::getContainer(int key) {
    return InputProcessor::getInstance().keyDownHandlers[key];
}

inline RefContainer<KeyHoldHandler>& KeyHoldHandler::getContainer(int key) {
    return InputProcessor::getInstance().keyHoldHandlers[key];
}

inline RefContainer<KeyReleaseHandler>& KeyReleaseHandler::getContainer(int key) {
    return InputProcessor::getInstance().keyReleaseHandlers[key];
}

} // minecpp

#endif // _MINECPP_INPUT_H_

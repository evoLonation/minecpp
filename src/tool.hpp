#ifndef _MINECPP_TOOL_H_
#define _MINECPP_TOOL_H_

#include <corecrt.h>
#include<vector>
#include<functional>
#include<set>
#include<map>
#include "exception.hpp"

namespace minecpp
{

class Defer{
private:
   std::function<void(void)> func;
public: 
   Defer(const std::function<void(void)>& func): func(func){}
   Defer(std::function<void(void)>&& func): func(std::move(func)){}
   ~Defer(){func();}
   
   // move semantic
   Defer& operator=(Defer&& ) = delete;
   Defer(Defer&& ) = delete;
   // copy semantic
   Defer& operator=(const Defer& ) = delete;
   Defer(const Defer& ) = delete;
};

// 自增的id
// 复制不会增加id
template<typename T>
class IdHolder{
private:
   static int currentMaxId;
   int id;
public:
   IdHolder(): id(++currentMaxId){}

   bool operator==(const IdHolder& right) const {
      return this->id == right.id;
   }
   bool operator<(const IdHolder& right) const {
      return this->id < right.id;
   }
   int getId() const {return id;}

   // copy semantic
   IdHolder& operator=(const IdHolder& ) = default;
   IdHolder(const IdHolder& ) = default;
   // move semantic
   IdHolder& operator=(IdHolder&& ) = default;
   IdHolder(IdHolder&& ) = default;

};
template<typename T>
inline int IdHolder<T>::currentMaxId = 0;

// 目前只支持移动存储类型，或者存储类型的引用
template<typename T>
class IdContainer{
private:
   using IdContainerBase = std::map<int, typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>>;
   IdContainerBase baseMap;
public:
   // using StoreType = typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>;
   using WithId = IdHolder<T>;
   // 当T不是引用类型时，add函数传入左值还是右值都可，因此两个函数都开启（第一个函数不是万能引用）；当T是引用类型时，只能传入左值，T&&是左值引用
   int add(T&& obj){
      WithId withId;
      baseMap.insert({withId.getId(), std::forward<T>(obj)});
      return withId.getId();
   }
   template <typename U = T>
   typename std::enable_if_t<!std::is_reference_v<U>, int> add(const U& obj) {
      WithId withId;
      baseMap.insert({withId.getId(), obj});
      return withId.getId();
   }
   void remove(int id){
      baseMap.erase(id);
   }
   // 同add函数
   template <typename U = T>
   typename std::enable_if_t<!std::is_reference_v<U>> replace(int id, const U& obj) {
      baseMap.erase(id);
      baseMap.insert({id, obj});
   }
   void replace(int id, T&& obj){
      baseMap.erase(id);
      baseMap.insert({id, std::forward<T>(obj)});
   }
   class Iterator{
   private:
      IdContainerBase::iterator baseIterator;
   public:
      Iterator(IdContainerBase::iterator iterator): baseIterator(iterator){}
      template <typename U = T>
      typename std::enable_if_t<!std::is_reference_v<U>, T&> operator*(){
         return baseIterator->second;
      }
      template <typename U = T>
      typename std::enable_if_t<std::is_reference_v<U>, T> operator*(){
         return baseIterator->second.get();
      }

      Iterator& operator++(){
         ++baseIterator;
         return *this;
      }
      // int 参数仅用于区分++运算符是前缀还是后缀，有int参数的是后缀
      Iterator operator++(int){
         Iterator old = *this;
         ++*this;
         return old;
      }
      bool operator!=(Iterator it){
         return baseIterator != it.baseIterator;
      }
   };
   Iterator begin(){
      return Iterator{baseMap.begin()};
   }
   Iterator end(){
      return Iterator{baseMap.end()};
   }
   IdContainerBase::size_type size(){
      return baseMap.size();
   }
   void clear(){
      baseMap.clear();
   }

   IdContainer() = default;
   // move semantic
   IdContainer& operator=(IdContainer&& ) = default;
   IdContainer(IdContainer&& ) = default;
   // copy semantic
   IdContainer& operator=(const IdContainer& ) = default;
   IdContainer(const IdContainer& ) = default;
};

// 赋予T类型以自动装填和卸载的功能
template<typename T>
class AutoLoad{
private:
   int id;
   bool moved {false};
   std::function<IdContainer<T&>&(T&)> containerGetter;
protected:
   AutoLoad(const std::function<IdContainer<T&>&(T&)>& containerGetter)
   :containerGetter(containerGetter){
      id = containerGetter(*static_cast<T*>(this)).add(*static_cast<T*>(this));
   }
   ~AutoLoad(){
      if(!moved){
         containerGetter(*static_cast<T*>(this)).remove(id);
      }
   }

   // copy and move constructor   
   AutoLoad(AutoLoad&& autoLoader): containerGetter(std::move(autoLoader.containerGetter)), id(autoLoader.id){
      autoLoader.moved = true;
      containerGetter(*static_cast<T*>(this)).replace(id, *static_cast<T*>(this));
   }
   AutoLoad(const AutoLoad& autoLoader): containerGetter(autoLoader.containerGetter) {
      id = containerGetter(*static_cast<T*>(this)).add(*static_cast<T*>(this));
   }
   // 赋值操作：不会发生任何变化
   AutoLoad& operator=(AutoLoad&& autoLoader) {
      // 没必要，因为两个对象分别在容器中的不同位置上，不像移动构造需要替换位置
      // autoLoader.moved = true;
      return *this;
   }
   AutoLoad& operator=(const AutoLoad& autoLoader) {
      return *this;
   }
   
};

// 没有参数，位于静态内存空间
template<typename T>
class Singleton{
protected:
   Singleton() = default;
public:
   static T& getInstance(){
      static T instance;
      return instance;
   }

   // deleted copy semantic
   Singleton& operator=(const Singleton&) = delete;
   Singleton(const Singleton&) = delete;
   // deleted move semantic
   Singleton& operator=(Singleton&&) = delete;
   Singleton(Singleton&&) = delete;
   
};

// need to construct proactively
// 需要主动构造，可含有参数，若在构造前调用getInstance()会抛异常
template<typename T>
class ProactiveSingleton{
private:
   static T* instancePtr;

   bool moved {false};
protected:
   // inherit class's "this" pointer call use in initialize list 
   // as long as not being used to access uninitialized members or virtual functions
   ProactiveSingleton(){
      if(instancePtr != nullptr){
         throwError("construct multi time!");
      }
      instancePtr = static_cast<T*>(this);
   }
   ~ProactiveSingleton(){
      if(!moved){
         instancePtr = nullptr;
      }
   }
public:
   static T& getInstance(){
      if(instancePtr == nullptr){
         throwError("need proactively construct first!");
      }
      return *instancePtr;
   }

   // 只能移动构造
   // deleted copy semantic
   ProactiveSingleton& operator=(const ProactiveSingleton&) = delete;
   ProactiveSingleton(const ProactiveSingleton&) = delete;
   // move semantic
   ProactiveSingleton& operator=(ProactiveSingleton&& ) = delete;
   ProactiveSingleton(ProactiveSingleton&& singleton) {
      singleton.moved = true;
      instancePtr = static_cast<T*>(this);
   }
};

template<typename T>
T* ProactiveSingleton<T>::instancePtr = nullptr;


class Observer;

// using ObserverId = int;
using ObservableId = int;
using ObserverContainer = IdContainer<Observer&>;
inline std::map<ObservableId, ObserverContainer> observersMap;

// 按照先后顺序保存一次链式调用中被改变的所有对象
// inline std::vector<std::function<void(void)>*> chainCall;
// inline std::set<std::function<void(void)>*> chainCallSet;

// 可观察对象
// 复制后，每个Observable副本的notice都会引起对应的所有Observer副本的相应
class Observable: protected IdHolder<Observable>{
friend class Observer;
private:
   ObserverContainer& getObservers() const{
      return observersMap[IdHolder<Observable>::getId()];
   }
public:
   Observable() = default;

   // 通知观察者
   void notice();
public:

   // default copy semantic
   Observable& operator=(const Observable&) = default;
   Observable(const Observable&) = default;
   // default move semantic
   Observable& operator=(Observable&&) = default;
   Observable(Observable&&) = default;
};

// 复制后，对每个Observer副本都会调用一次
class Observer: public AutoLoad<Observer>{
friend class Observable;
private:
   ObservableId observableId;
   std::function<void(void)> observerHandler;
   ObserverContainer& getObservers() const{
      return observersMap[observableId];
   }
public:
   Observer(const std::function<void(void)>& observerHandler, const Observable& observable)
   : observableId(observable.getId()), observerHandler(observerHandler),
   AutoLoad<Observer>([] (auto& obj) -> auto& {
      return obj.getObservers();
   }){}
   ~Observer() = default;

   // default move semantic
   Observer& operator=(Observer&&) = default;
   Observer(Observer&&) = default;

   // default copy semantic
   Observer& operator=(const Observer&) = default;
   Observer(const Observer&) = default;
   
};

inline void Observable::notice(){
   for(auto& observer : getObservers()){
      observer.observerHandler();
   }
}


template<typename T, typename... Args>
class ReactiveValue;

template<typename T>
class ObservableValue: public Observable{

   template<typename TT, typename... Args>
   friend class ReactiveValue;

private:
   // 第一层先定位到某些相同id的 observablevalue（由同一个拷贝多份而来）， 第二层定位到某些相同id的 reactiveValue，存放所有需要自己的指针的位置的map
   static std::map<int, std::map<int, ObservableValue**>> pointerLocationMap;

protected:
   T mValue;

   // 默认初始化
   ObservableValue() = default;
   // 由一个值初始化
   ObservableValue(const T& t)noexcept:mValue(t){}
   ObservableValue(T&& t)noexcept:mValue(std::move(t)){}
   
   void notice(){
      for(auto [_, pointerLocation]: pointerLocationMap[this->getId()]){
         *pointerLocation = this;
      }
      Observable::notice();
   }
public:

   // default move semantic
   ObservableValue& operator=(ObservableValue&&) = default;
   ObservableValue(ObservableValue&&) = default;
   // default copy semantic
   ObservableValue& operator=(const ObservableValue&) = default;
   ObservableValue(const ObservableValue&) = default;

   operator const T&() const {return get();}

   const T& get()const noexcept{ return mValue; }
};

template<typename T>
inline std::map<int, std::map<int, ObservableValue<T>**>> ObservableValue<T>::pointerLocationMap;

// 手动调用notice提示数值变化
template<typename T>
class ChangeableObservable: public ObservableValue<T>{
private:
   T oldValue;
public:
   ChangeableObservable() = default;
   ChangeableObservable(const T& t)noexcept:ObservableValue<T>(t), oldValue(t){}
   ChangeableObservable(T&& t)noexcept:ObservableValue<T>(std::move(t)), oldValue(this->mValue){}

   bool mayNotice(){
      if(oldValue != this->mValue){
         this->notice();
         oldValue = this->mValue;
         return true;
      }
      return false;
   }
   T& operator*(){return val();}
   T* operator->() {return &this->val();}
   T& val()noexcept{ return this->mValue; }

   ChangeableObservable& operator=(const T& t)noexcept{
      this->mValue = t;
      this->mayNotice();
      return *this;
   }
   ChangeableObservable& operator=(T&& t)noexcept{
      this->mValue = std::move(t);
      this->mayNotice();
      return *this;
   }

   // default move semantic
   ChangeableObservable& operator=(ChangeableObservable&&) = default;
   ChangeableObservable(ChangeableObservable&&) = default;
   // default copy semantic
   ChangeableObservable& operator=(const ChangeableObservable&) = default;
   ChangeableObservable(const ChangeableObservable&) = default;
   
};

template<typename T, typename... Args>
class ReactiveValue: public ObservableValue<T>{
private:
   std::function<T(const Args&...)> computeFunc;
   std::tuple<const ObservableValue<Args>*...> args;
   std::array<Observer,sizeof...(Args)>  observers;

private:
   T updateValue(){
      this->mValue = std::apply(this->computeFunc, std::apply([](const ObservableValue<Args>*... args){return std::make_tuple(args->get()...);}, this->args));
   }
   template<typename Arg>
   Observer createObserver(const ObservableValue<Arg>& arg){
      return {
         [this](){
            updateValue();
            this->notice();
         },
         arg
      };
   }
public:
   template<typename Callable>
   ReactiveValue(Callable&& computeFunc, const ObservableValue<Args>&... args):
   computeFunc(std::forward<Callable>(computeFunc)),
   args{&args...},
   observers{createObserver(args)...}
   {
      (args.pointerLocationMap[args.getId()].insert({this->getId(), nullptr}), ...);
      updateValue();
   }

   // 拷贝赋值函数和拷贝构造函数，包括 index_sequence , std::get, make_tuple 等等的使用
private:
   template<std::size_t ...N>
   ReactiveValue& assign(const ReactiveValue& value, std::index_sequence<N...> int_seq){
      computeFunc = value.computeFunc;
      args = value.args;
      observers = {createObserver(*std::get<N>(args))...};
      updateValue();
   }
   template<std::size_t ...N>
   ReactiveValue(const ReactiveValue& value, std::index_sequence<N...> int_seq):
   computeFunc(value.computeFunc), args(value.args), observers{createObserver(*std::get<N>(args))...}{
      updateValue();
   }
   // 移动和拷贝均基于上面的版本
public:   
   ReactiveValue& operator=(const ReactiveValue& value){
      assign(value, std::make_index_sequence<sizeof...(Args)>{});
      return *this;
   }
   ReactiveValue& operator=(ReactiveValue&& value){
      return operator=(value);
   }
   ReactiveValue(const ReactiveValue& value): ReactiveValue(value, std::make_index_sequence<sizeof...(Args)>{}){}
   ReactiveValue(ReactiveValue&& value): ReactiveValue(value){}

   const T& operator*()const {return this->get();}
   const T* operator->()const {return &this->get();}
};

// template<typename ...Args>
// class ReactiveObserver{
// private:
//    std::function<void(const Args&...)> computeFunc;
//    std::tuple<const ObservableValue<Args>*...> args;
//    Observer observers[sizeof...(Args)];
// public:
//    template<typename Callable>
//    ReactiveObserver(Callable&& computeFunc, const ObservableValue<Args>&... args):
//    computeFunc(std::forward<Callable>(computeFunc)),
//    args{&args...},
//    observers{
//       Observer{args, [this, &args...](){
//          this->computeFunc(args.get()...);
//       }}...
//    }{}

//    // 拷贝构造函数，包括 index_sequence , std::get, make_tuple 等等的使用
//    template<size_t ...N>
//    ReactiveObserver(const ReactiveObserver& value, std::index_sequence<N...> int_seq):
//    computeFunc(value.computeFunc),
//    args(value.args),
//    observers{
//       Observer{*std::get<N>(args), [this](){
//          std::apply(this->computeFunc, std::apply([](const ObservableValue<Args>*... args){return std::make_tuple(args->get()...);}, this->args));
//       }}...
//    }
//    {}
//    ReactiveObserver(const ReactiveObserver& value): ReactiveObserver(value, std::make_index_sequence<sizeof...(Args)>{}){}
   
//    // 移动构造，直接用拷贝构造代替
//    ReactiveObserver(ReactiveObserver&& value) = delete;
// };

template<typename Callable, typename... Args>
ReactiveValue<std::invoke_result_t<Callable, Args...>, Args...> makeReactiveValue(Callable&& computeFunc, ObservableValue<Args>&... args){
   return ReactiveValue<std::invoke_result_t<Callable, Args...>,  Args...>(std::forward<Callable>(computeFunc), args...);
}

// template<typename Callable, typename... Args>
// class ReactiveValueAuto: public ReactiveValue<std::invoke_result_t<Callable, Args...>, Args...>{
// using ValueType = std::invoke_result_t<Callable, Args...>;
// public:
//    ReactiveValueAuto(Callable&& computeFunc, ObservableValue<Args>&... args)
//    :ReactiveValue<ValueType, Callable, Args...>(std::forward<Callable>(computeFunc), args...){}
// };

} // namespace minecpp

#endif // _MINECPP_TOOL_H_
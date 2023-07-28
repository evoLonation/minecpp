#ifndef _MINECPP_TOOL_H_
#define _MINECPP_TOOL_H_

#include <tuple>
#include <vector>
#include <functional>
#include <set>
#include <map>
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
   Defer(Defer&& ) = default;
   // copy semantic
   Defer& operator=(const Defer& ) = delete;
   Defer(const Defer& ) = delete;
};

// 自增的id
// id永远是唯一的
// 复制同样会增加id
template<typename T>
class IdHolder{
private:
   using IDType = int;

   static IDType currentMaxId;
   IDType id;

protected:

   IdHolder(): id(++currentMaxId) {}
   ~IdHolder() = default;

   bool operator==(const IdHolder& right) const {
      return this->id == right.id;
   }
   bool operator<(const IdHolder& right) const {
      return this->id < right.id;
   }

   IDType getId() const {return id;}

   // copy semantic
   IdHolder& operator=(const IdHolder& idHolder) {
      id = ++currentMaxId;
      return *this;
   }
   IdHolder(const IdHolder& idHolder): IdHolder(){}
   // 移动会将id拥有权转过来
   IdHolder& operator=(IdHolder&& idHolder) {
      id = idHolder.id;
      idHolder.id = 0;
   }
   IdHolder(IdHolder&& idHolder){
      id = idHolder.id;
      idHolder.id = 0;
   }
};
template<typename T>
inline  IdHolder<T>::IDType IdHolder<T>::currentMaxId = 0;

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

// 对象或者其引用的无序容器，并能够在插入时产生对应的id
template<typename T>
class IdContainer{
private:
   using IdContainerBase = std::map<int, typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>>;
   IdContainerBase baseMap;
public:
   // using StoreType = typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>;
   class WithId: public IdHolder<WithId>{
   public:
      using IdHolder<WithId>::getId;
   };
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
   class ConstIterator{
   private:
      IdContainerBase::const_iterator baseIterator;
   public:
      ConstIterator(IdContainerBase::const_iterator iterator): baseIterator(iterator){}
      template <typename U = T>
      typename std::enable_if_t<!std::is_reference_v<U>, const T&> operator*() const {
         return baseIterator->second;
      }
      template <typename U = T>
      typename std::enable_if_t<std::is_reference_v<U>, const std::remove_reference_t<T>&> operator*() const {
         return baseIterator->second.get();
      }

      ConstIterator& operator++(){
         ++baseIterator;
         return *this;
      }
      // int 参数仅用于区分++运算符是前缀还是后缀，有int参数的是后缀
      ConstIterator operator++(int){
         ConstIterator old = *this;
         ++*this;
         return old;
      }
      bool operator!=(ConstIterator it){
         return baseIterator != it.baseIterator;
      }
   };
   Iterator begin(){
      return Iterator{baseMap.begin()};
   }
   Iterator end(){
      return Iterator{baseMap.end()};
   }
   ConstIterator begin() const {
      return ConstIterator{baseMap.begin()};
   }
   ConstIterator end() const {
      return ConstIterator{baseMap.end()};
   }
   IdContainerBase::size_type size(){
      return baseMap.size();
   }
   Iterator find(int id){
      return baseMap.find(id);
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


// AutoLoader和RefContainer两个分别持有容器的控制权和使用权
template<typename T>
class AutoLoader;

template<typename T>
class RefContainer: private IdContainer<T&>{
friend class AutoLoader<T>;
public:
   using IdContainer<T&>::begin;
   using IdContainer<T&>::end;
   using IdContainer<T&>::size;
   RefContainer() = default;

   // 拷贝和移动时什么都不会干，因为容器内容的控制权不在本对象手里
   RefContainer& operator=(RefContainer&& container) {
      return *this;
   }
   RefContainer& operator=(const RefContainer&) {
      return *this;
   }
   RefContainer(const RefContainer&) {}
   RefContainer(RefContainer&& container) {}
   
};

// 赋予T类型以自动装填和卸载的功能
// 若被其他类继承，则不同于主动传入，需要在子类的内存位置变动时也跟着变动
// 该类拥有的资源：对目标容器中一个位置的控制权
template<typename T>
class AutoLoader{
private:
   static consteval bool isBase() {
      return std::is_base_of<AutoLoader<T>, T>::value;
   }
   T& getMaster(){
      return *static_cast<T*>(this);
   }
   int id;
   bool moved {false};
   RefContainer<T>* container;
protected:
   template<typename U = T> requires (isBase())
   AutoLoader(RefContainer<T>& container): container(&container){
      id = container.add(getMaster());
   }
public:
   // 判断模板实例化后是不是类型参数T的直接父类
   template<typename U = T> requires (!isBase())
   AutoLoader(RefContainer<T>& container, T& e) 
   :container(&container){
      id = container.add(e);
   }

   ~AutoLoader(){
      if(!moved){
         container->remove(id);
      }
   }

   // copy and move constructor
   AutoLoader(AutoLoader&& autoLoader): container(autoLoader.container), id(autoLoader.id){
      autoLoader.moved = true;
      if constexpr (isBase()){
         container->replace(id, getMaster());
      }
   }
   AutoLoader(const AutoLoader& autoLoader): container(autoLoader.container) {
      if constexpr (isBase()) {
         id = container->add(getMaster());
      }else{
         id = container->add(container->find(autoLoader.id));
      }
   }
   // 移动赋值：自己的移除，并接管入参的
   AutoLoader& operator=(AutoLoader&& autoLoader) {
      if(!moved){
         container->remove(id);
      }else{
         moved = false;
      }
      container = autoLoader.container;
      id = autoLoader.id;
      autoLoader.moved = true;
      if constexpr (isBase()){
         container->replace(id, getMaster());
      }
      return *this;
   }
   // 拷贝赋值：自己的移除，并根据入参加载相同元素
   AutoLoader& operator=(const AutoLoader& autoLoader) {
      if(!moved){
         container->remove(id);
      }else{
         moved = false;
      }
      container = autoLoader.container;
      if constexpr (isBase()) {
         id = container->add(getMaster());
      }else{
         id = container->add(container->find(autoLoader.id));
      }
      return *this;
   }
   
};

// Observable 与 Observer ，是RefContainer和AutoLoader的典型代表

template<typename... Args>
class AbstractObserver;

// 赋值操作 什么都不会发生
// 无论怎么初始化，都会获得一个空的 observer 列表
template<typename... Args>
class Observable {
friend class AbstractObserver<Args...>;
private:
   RefContainer<AbstractObserver<Args...>> observers;

protected:
   Observable() = default;
   ~Observable() = default;

   // 通知观察者
   void notify(Args... args);

   // 赋值时要求 被赋值者 未关联任何 Observer
   Observable& operator=(const Observable& observable) = default;
   Observable& operator=(Observable&& observable) = default;
   Observable(const Observable& observable) = default;
   Observable(Observable&& observable) = default;
};

// 拷贝语义实现同一个事件重复调用多次
template<typename... Args>
class AbstractObserver: public AutoLoader<AbstractObserver<Args...>>{
friend class Observable<Args...>;
protected:
   virtual void handle(Args... arg) = 0;
   AbstractObserver(Observable<Args...>& observable):
      AutoLoader<AbstractObserver<Args...>>(observable.observers){}

   ~AbstractObserver() = default;

   // default move semantic
   AbstractObserver& operator=(AbstractObserver&&) = default;
   AbstractObserver(AbstractObserver&&) = default;
   // default copy semantic
   AbstractObserver& operator=(const AbstractObserver&) = default;
   AbstractObserver(const AbstractObserver&) = default;
};

template<typename... Args>
void Observable<Args...>::notify(Args... args) {
   for(auto& observer : observers){
      observer.handle(args...);
   }
}

template<typename... Args>
class Observer: public AbstractObserver<Args...>{
private:
   std::function<void(Args...)> handler;
public:
   void handle(Args... arg) override {
      handler(arg...);
   }
   template<typename Callable>
   Observer(Callable handler, Observable<Args...>& observable)
   :  AbstractObserver<Args...>(observable), handler(std::forward<Callable>(handler)){}

   ~Observer() = default;

   // default move semantic
   Observer& operator=(Observer&&) = default;
   Observer(Observer&&) = default;
   // default copy semantic
   Observer& operator=(const Observer&) = default;
   Observer(const Observer&) = default;

   // 本质上是可调用的对象
   template<typename Callable>
   void setHandler(Callable handler) {
      this->handler = std::forward<Callable>(handler);
   }
   
};

// 手动调用notice提示数值变化
// 赋值操作、拷贝与移动初始化操作 只会拷贝值
template<typename T>
class ObservableValue: public Observable<const T&>{
private:
   T mValue;
   T oldValue;
public:
   ObservableValue() = default;
   
   bool mayNotify(){
      if(oldValue != this->mValue){
         this->notify(mValue);
         oldValue = this->mValue;
         return true;
      }
      return false;
   }

   ObservableValue(const T& t):mValue(t), oldValue(t){}
   ObservableValue(T&& t):mValue(std::move(t)), oldValue(this->mValue){}

   ObservableValue& operator=(const T& t)noexcept{
      this->mValue = t;
      this->mayNotify();
      return *this;
   }
   ObservableValue& operator=(T&& t)noexcept{
      this->mValue = std::move(t);
      this->mayNotify();
      return *this;
   }

   const T& get()const noexcept{ return mValue; }
   operator const T&() const {return get();}
   T& operator*(){return val();}
   T* operator->() {return &this->val();}
   T& val()noexcept{ return this->mValue; }

   // default move semantic
   ObservableValue& operator=(ObservableValue&&) = default;
   ObservableValue(ObservableValue&&) = default;
   // default copy semantic
   ObservableValue& operator=(const ObservableValue&) = default;
   ObservableValue(const ObservableValue&) = default;
   
};

// 拷贝操作和移动操作后，该值的响应式仍然存在
template<typename T, typename... Args>
class ReactiveValue: public ObservableValue<T>{
friend class ReactiveValue<T, Args...>;
private:
   template<typename Arg>
   class ValueObserver: public AbstractObserver<const Arg&>{
   public:
      ValueObserver(ObservableValue<Arg>& argObservable, ReactiveValue& father): 
         AbstractObserver<const Arg &>(argObservable), arg(argObservable.get()), father(&father){}
      
      Arg arg;
      ReactiveValue* father;
      void handle(const Arg& arg) override {
         this->arg = arg;
         father->updateValue();
      }
   };
   std::function<T(const Args&...)> computeFunc;
   std::tuple<ValueObserver<Args>...>  observers;
   void computeValue(ValueObserver<Args>&...  observers){
      ObservableValue<T>::operator=(computeFunc(observers.arg...));
   }
   void updateValue(){
      // std::apply 用于将tuple展开作为参数传入函数
      // 调用类的成员函数：在前面加&Class, 并需要 bind_front
      std::apply(std::bind_front(&ReactiveValue::computeValue, this), observers);
   }

public:
   template<typename Callable>
   ReactiveValue(Callable&& computeFunc, ObservableValue<Args>&... args):
      computeFunc(std::forward<Callable>(computeFunc)),
      observers{ValueObserver<Args>(args, *this)...}
   {
      updateValue();
   }

   void updateFatherPointer(){
      // 使用 std::apply 实现 tuple 的遍历执行
      std::apply([this](auto&... args) {((args.father = this), ...);}, observers);
   }
   ReactiveValue(ReactiveValue&& observer):
      ObservableValue<T>(std::move(observer)),
      computeFunc(std::move(observer.computeFunc)),
      observers{std::move(observer.observers)}
   {
      updateFatherPointer();
   }
   ReactiveValue(const ReactiveValue& observer):
      ObservableValue<T>(observer),
      computeFunc(observer.computeFunc),
      observers{observer.observers}
   {
      updateFatherPointer();
   }
   // move semantic
   ReactiveValue& operator=(ReactiveValue&& observer) {
      ObservableValue<T>::operator=(std::move(observer));
      computeFunc = std::move(observer.computeFunc);
      observers = std::move(observer.observers);
      updateFatherPointer();
   }
   // deleted copy semantic
   ReactiveValue& operator=(const ReactiveValue& observer) {
      ObservableValue<T>::operator=(observer);
      computeFunc = observer.computeFunc;
      observers = observer.observers;
      updateFatherPointer();
   }
};

template<typename Callable, typename... Args>
ReactiveValue<std::invoke_result_t<Callable, Args...>, Args...> makeReactiveValue(Callable&& computeFunc, ObservableValue<Args>&... args){
   return ReactiveValue<std::invoke_result_t<Callable, Args...>,  Args...>(std::forward<Callable>(computeFunc), args...);
}

// todo
template<typename T, typename... Args>
class ReactiveBinder{
public:
   template<typename Callable>
   ReactiveBinder(Callable&& computeFunc, ObservableValue<T>& target, ObservableValue<Args>&... sources){}
};

} // namespace minecpp

#endif // _MINECPP_TOOL_H_
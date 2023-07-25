#ifndef _MINECPP_TOOL_H_
#define _MINECPP_TOOL_H_

#include<vector>
#include<functional>
#include<set>
#include<map>
#include "exception.hpp"

namespace minecpp
{

// 其他继承该类的类默认是无法拷贝的，除非自己定义；但是是可以移动的
class UnCopyable{
public:
   UnCopyable() = default;
   UnCopyable& operator=(UnCopyable const &) = delete;
   UnCopyable(UnCopyable const &) = delete;
   UnCopyable& operator=(UnCopyable &&) = default;
   UnCopyable(UnCopyable &&) = default;
};

// 其他继承该类的类默认是无法移动/拷贝赋值的
class UnAssignable{
public:
   UnAssignable() = default;
   UnAssignable& operator=(UnAssignable const &) = delete;
   UnAssignable& operator=(UnAssignable &&) = delete;
};

// 继承该类的类默认是无法拷贝和移动的
class UnCopyMoveable: public UnCopyable{
public:
   UnCopyMoveable() = default;
   UnCopyMoveable& operator=(UnCopyMoveable &&) = delete;
   UnCopyMoveable(UnCopyMoveable &&) = delete;
};
// 没有UnMoveable是因为要想禁止移动就必须也把拷贝语义禁止


class Defer: public UnCopyMoveable{
private:
   std::function<void(void)> func;
public: 
   Defer(const std::function<void(void)>& func): func(func){}
   Defer(std::function<void(void)>&& func): func(std::move(func)){}
   ~Defer(){func();}
};

template<typename T>
class IdHolder: public UnCopyable{
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
};

template<typename T>
class AutoLoad: UnCopyable{
private:
   int id;
   bool moved {false};
   std::function<IdContainer<T&>&(void)> containerGetter;
public:
   AutoLoad(const std::function<IdContainer<T&>&(void)>& containerGetter)
   :containerGetter(containerGetter){
      id = containerGetter().add(*static_cast<T*>(this));
   }
   ~AutoLoad(){
      if(!moved){
         containerGetter().remove(id);
      }
   }
   AutoLoad(AutoLoad&& a){
      a.moved = true;
      id = a.id;
      containerGetter = a.containerGetter;
      containerGetter().replace(id, *static_cast<T*>(this));
   }
   AutoLoad& operator=(AutoLoad&& a) = delete;
};


template<typename T>
class Singleton: public UnCopyMoveable{
protected:
   Singleton() = default;
public:
   static T& getInstance(){
      static T instance;
      return instance;
   }
};

// need to construct proactively
template<typename T>
class ProactiveSingleton: public UnCopyMoveable{
private:
   static T* instancePtr;
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
      instancePtr = nullptr;
   }
public:
   static T& getInstance(){
      if(instancePtr == nullptr){
         throwError("need proactively construct first!");
      }
      return *instancePtr;
   }
};

template<typename T>
T* ProactiveSingleton<T>::instancePtr = nullptr;

// 按照先后顺序保存一次链式调用中被改变的所有对象
inline std::vector<std::function<void(void)>*> chainCall;
inline std::set<std::function<void(void)>*> chainCallSet;

// 可观察对象
class Observable: public IdHolder<Observable>{
private:
   using ObserverContainer = std::map<int, std::function<void(void)>>;
   static std::map<int, ObserverContainer> observersMap;
   ObserverContainer& getObservers() const{
      return observersMap[IdHolder<Observable>::getId()];
   }
public:
   // abstract class
   Observable() = default;

   // 通知观察者，会防止循环调用 
   void notice(){
      for(auto& pair : getObservers()){
         auto& observer = pair.second;
         // 如果调用链中包含此对象，说明发生了循环调用，直接返回
         if(chainCallSet.contains(&observer)){
            continue;
         }
         // 在调用链中添加自己
         chainCall.push_back(&observer);
         chainCallSet.insert(&observer);
         // 其中也可能发生调用
         observer();
         // 在调用链中删除自己
         chainCall.pop_back();
         chainCallSet.erase(&observer);
      }
   }
private:
   class Handler: public IdHolder<Handler>{};
public:
   int addObserver(const std::function<void(void)>& observer)const{
      Handler handler;
      getObservers().insert({handler.getId(), observer});
      return handler.getId();
   }
   void removeObserver(int id)const{
      getObservers().erase(id);
   }
};

inline std::map<int, std::map<int, std::function<void(void)>>> Observable::observersMap;

class Observer: public UnCopyable{
private:
   int id;
   const Observable& observable;
public:
   Observer(const Observable& observable, const std::function<void(void)>& observer): observable(observable){
      this->id = observable.addObserver(observer);
   }
   Observer(Observer&& obj) = default;
   ~Observer(){
      observable.removeObserver(id);
   }
};



// 封装数据，当调用check时检查数据距离初始化或者上次check时是否变化，如果变化则通知观察者
template<typename T>
class RefDirtyObservable: public Observable{
private:
   T mValue;
   T oldValue;
public:
   // same as dirtyobserable
   RefDirtyObservable(const T& t)noexcept:mValue(t), oldValue(t){}
   RefDirtyObservable(T&& t)noexcept:mValue(std::move(t)), oldValue(mValue){}
   
   T& value()noexcept{return mValue;}

   void check(){
      if(oldValue != mValue){
         oldValue = mValue;
         notice();
      }
   }
};

template<typename T>
class ObservableValue: public Observable{
protected:
   T mValue;
   // 初始化
   // 默认初始化
   ObservableValue() = default;
   // 由一个值初始化
   ObservableValue(const T& t)noexcept:mValue(t){}
   ObservableValue(T&& t)noexcept:mValue(std::move(t)){}
   // 由同类型对象拷贝初始化（用 T的构造函数 + 对T的类型转换来代替）
   // ObservableValue(const ObservableValue& t)noexcept:ObservableValue(t.get()){}
   // 移动初始化无法被代替
   // 必须存在至少一个显示的拷贝或者移动初始化，否则无法存入标准库容器中
   ObservableValue(ObservableValue&& t)noexcept:ObservableValue(std::move(t.mValue)){}
public:
   ObservableValue& operator=(const ObservableValue& t)=delete;
   ObservableValue& operator=(ObservableValue&& t)=delete;

   operator const T&() const {return get();}

   const T& get()const noexcept{ return mValue; }
};

// 手动调用notice提示数值变化
template<typename T>
class ChangeableObservable: public ObservableValue<T>{
private:
   T oldValue;
public:
   ChangeableObservable() = default;
   ChangeableObservable(const T& t)noexcept:ObservableValue<T>(t), oldValue(t){}
   ChangeableObservable(T&& t)noexcept:ObservableValue<T>(std::move(t)), oldValue(this->mValue){}
   ChangeableObservable(ChangeableObservable&& t)noexcept:ObservableValue<T>(std::move(t)), oldValue(std::move(t.oldValue)){}


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
   ChangeableObservable& operator=(const ChangeableObservable& t)noexcept{
      this->operator=(t.get());
      return *this;
   }
   ChangeableObservable& operator=(ChangeableObservable&& t)noexcept{
      this->operator=(std::move(t.mValue));
      return *this;
   }
};

template<typename T, typename... Args>
class ReactiveValue: public ObservableValue<T>, UnAssignable{
// using ValueType = std::invoke_result_t<Callable, Args...>;
private:
   std::function<T(const Args&...)> computeFunc;
   std::tuple<const ObservableValue<Args>*...> args;
   Observer observers[sizeof...(Args)];
public:
   template<typename Callable>
   ReactiveValue(Callable&& computeFunc, const ObservableValue<Args>&... args):
   computeFunc(std::forward<Callable>(computeFunc)),
   args{&args...},
   observers{
      Observer{args, [this, &args...](){
         this->mValue = this->computeFunc(args.get()...);
         this->notice();
      }}...
   }
   {
      this->mValue = computeFunc(args.get()...);
   }

   // 拷贝构造函数，包括 index_sequence , std::get, make_tuple 等等的使用
   template<size_t ...N>
   ReactiveValue(const ReactiveValue& value, std::index_sequence<N...> int_seq):
   computeFunc(value.computeFunc),
   args(value.args),
   observers{
      Observer{*std::get<N>(args), [this](){
         this->mValue = std::apply(this->computeFunc, std::apply([](const ObservableValue<Args>*... args){return std::make_tuple(args->get()...);}, this->args));
         this->notice();
      }}...
   }
   {
      this->mValue = std::apply(this->computeFunc, std::apply([](const ObservableValue<Args>*... args){return std::make_tuple(args->get()...);}, this->args));
   }
   ReactiveValue(const ReactiveValue& value): ReactiveValue(value, std::make_index_sequence<sizeof...(Args)>{}){}
   
   // 移动构造，直接用拷贝构造代替（不要声明为已删除，这样的话对其移动会编译错误）
   // ReactiveValue(ReactiveValue&& value) = delete;
   // ReactiveValue(ReactiveValue&& value): ReactiveValue(value){}
   const T& operator*()const {return this->get();}
   const T* operator->()const {return &this->get();}
};

template<typename ...Args>
class ReactiveObserver{
private:
   std::function<void(const Args&...)> computeFunc;
   std::tuple<const ObservableValue<Args>*...> args;
   Observer observers[sizeof...(Args)];
public:
   template<typename Callable>
   ReactiveObserver(Callable&& computeFunc, const ObservableValue<Args>&... args):
   computeFunc(std::forward<Callable>(computeFunc)),
   args{&args...},
   observers{
      Observer{args, [this, &args...](){
         this->computeFunc(args.get()...);
      }}...
   }{}

   // 拷贝构造函数，包括 index_sequence , std::get, make_tuple 等等的使用
   template<size_t ...N>
   ReactiveObserver(const ReactiveObserver& value, std::index_sequence<N...> int_seq):
   computeFunc(value.computeFunc),
   args(value.args),
   observers{
      Observer{*std::get<N>(args), [this](){
         std::apply(this->computeFunc, std::apply([](const ObservableValue<Args>*... args){return std::make_tuple(args->get()...);}, this->args));
      }}...
   }
   {}
   ReactiveObserver(const ReactiveObserver& value): ReactiveObserver(value, std::make_index_sequence<sizeof...(Args)>{}){}
   
   // 移动构造，直接用拷贝构造代替
   ReactiveObserver(ReactiveObserver&& value) = delete;
};

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
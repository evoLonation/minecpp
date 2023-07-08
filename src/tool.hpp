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
class IdContainer: public std::map<int, typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>>{
public:
   // using StoreType = typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>;
   using WithId = IdHolder<T>;
   int add(T&& obj){
      WithId withId;
      this->insert({withId.getId(), std::forward<T>(obj)});
      return withId.getId();
   }
   template <typename U = T>
   typename std::enable_if_t<!std::is_reference_v<U>, int> add(const U& obj) {
      WithId withId;
      this->insert({withId.getId(), obj});
      return withId.getId();
   }
   void remove(int id){
      this->erase(id);
   }
   
   void forEach(std::function<void(T&)> handler){
      for(auto& pair: *this){
         auto& second = pair.second;
         handler(second);
      }
   }
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
   ProactiveSingleton(T* father){
      if(instancePtr != nullptr){
         throwError("construct multi time!");
      }
      instancePtr = father;
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
public:
   // 初始化
   // 默认初始化
   ObservableValue() = default;
   // 由一个值初始化
   ObservableValue(const T& t)noexcept:mValue(t){}
   ObservableValue(T&& t)noexcept:mValue(std::move(t)){}
   // 由同类型对象拷贝初始化
   ObservableValue(const ObservableValue& t)noexcept:ObservableValue(t.get()){}
   ObservableValue(ObservableValue&& t)noexcept:ObservableValue(std::move(t.mValue)){}
   ObservableValue& operator=(const ObservableValue& t)=delete;
   ObservableValue& operator=(ObservableValue&& t)=delete;

   operator const T&() const {return get();}

   const T& get()const noexcept{ return mValue; }
};

// 将对于观察者的通知封装到对数据的赋值操作中
template<typename T>
class AssignObservable: public ObservableValue<T>{
public:
   AssignObservable(const T& t)noexcept:ObservableValue<T>(t){}
   AssignObservable(T&& t)noexcept:ObservableValue<T>(std::move(t)){}
   
   AssignObservable& operator=(const T& t)noexcept{
      this->mValue = t;
      this->notice();
      return *this;
   }
   AssignObservable& operator=(T&& t)noexcept{
      this->mValue = std::move(t);
      this->notice();
      return *this;
   }
   AssignObservable& operator=(const AssignObservable& t)noexcept{
      this->operator=(t.get());
      return *this;
   }
   AssignObservable& operator=(AssignObservable&& t)noexcept{
      this->operator=(std::move(t.mValue));
      return *this;
   }
   AssignObservable(const AssignObservable& t)noexcept = default;
   AssignObservable(AssignObservable&& t)noexcept = default;
   const T& operator*()const {return this->get();}
   const T* operator->() {return &this->get();}
};
// 手动调用notice提示数值变化
template<typename T>
class ManualObservable: public ObservableValue<T>, public UnCopyMoveable{
private:
   T oldValue;
public:
   ManualObservable(const T& t)noexcept:ObservableValue<T>(t), oldValue(t){}
   ManualObservable(T&& t)noexcept:ObservableValue<T>(std::move(t)), oldValue(this->mValue){}

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
};

template<typename T, typename... Args>
class ReactiveValue: public ObservableValue<T>{
// using ValueType = std::invoke_result_t<Callable, Args...>;
private:
   Observer observers[sizeof...(Args)];
public:
   template<typename Callable>
   ReactiveValue(Callable&& computeFunc, const ObservableValue<Args>&... args)
   :observers{
      Observer{args, [this, computeFunc = std::forward<Callable>(computeFunc), &args...](){
         this->mValue = computeFunc(args.get()...);
         this->notice();
      }}...
   }{
      this->mValue = computeFunc(args.get()...);
   }
   const T& operator*()const {return this->get();}
   const T* operator->()const {return &this->get();}
};

template<typename Callable, typename... Args>
class ReactiveValueAuto: public ReactiveValue<std::invoke_result_t<Callable, Args...>, Args...>{
using ValueType = std::invoke_result_t<Callable, Args...>;
public:
   ReactiveValueAuto(Callable&& computeFunc, ObservableValue<Args>&... args)
   :ReactiveValue<ValueType, Args...>(std::forward<Callable>(computeFunc), args...){}
};

} // namespace minecpp

#endif // _MINECPP_TOOL_H_
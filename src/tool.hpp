#ifndef _MINECPP_TOOL_H_
#define _MINECPP_TOOL_H_

#include<vector>
#include<functional>
#include<set>
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



class Defer: public UnCopyable{
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
   // IdHolder& operator=(IdHolder const &) = delete;
   // IdHolder(IdHolder const &) = delete;
   // 如果想要移动语义不想要拷贝语义，必须显示声明移动语义
   IdHolder& operator=(IdHolder &&) = default;
   IdHolder(IdHolder &&) = default;

   bool operator==(const IdHolder& right) const {
      return this->id == right.id;
   }
   bool operator<(const IdHolder& right) const {
      return this->id < right.id;
   }
   int getId(){return id;}
};
template<typename T>
inline int IdHolder<T>::currentMaxId = 0;



template<typename T>
class Singleton{
protected:
   Singleton() = default;
public:
   Singleton& operator=(Singleton const &) = delete;
   Singleton(Singleton const &) = delete;
   static T& getInstance(){
      static T instance;
      return instance;
   }

};

// need to construct proactively
template<typename T>
class ProactiveSingleton{
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
public:
   ProactiveSingleton& operator=(ProactiveSingleton const &) = delete;
   ProactiveSingleton(ProactiveSingleton const &) = delete;
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
class Observable{
private:
   std::map<int, std::function<void(void)>> observers;
protected:
   // abstract class
   Observable() noexcept = default;

   // 通知观察者，会防止循环调用 
   void notice(){
      for(auto& pair : observers){
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
   class Handler:public IdHolder<Handler>{};
public:
   int addObserver(const std::function<void(void)>& observer)noexcept{
      Handler handler;
      observers.insert({handler.getId(), observer});
      return handler.getId();
   }
   void removeObserver(int id){
      observers.erase(id);
   }
};

class Observer: public UnCopyable{
private:
   int id;
   Observable& observable;
public:
   Observer(Observable& observable, const std::function<void(void)>& observer): observable(observable){
      this->id = observable.addObserver(observer);
   }
   ~Observer(){
      observable.removeObserver(id);
   }
};

// 将对于观察者的通知封装到对数据的赋值操作中
template<typename T>
class DirtyObservable: public Observable{
private:
   T mValue;
public:
   DirtyObservable(const T& t)noexcept:mValue(t){}
   DirtyObservable(T&& t)noexcept:mValue(std::move(t)){}
   
   DirtyObservable& operator=(const T& t)noexcept{
      mValue = t;
      notice();
      return *this;
   }
   DirtyObservable& operator=(T&& t)noexcept{
      mValue = std::move(t);
      notice();
      return *this;
   }

   const T& value()const noexcept{ return mValue; }
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
class BaseValue: public Observable{
protected:
   T mValue;
public:
   BaseValue() = default;
   BaseValue(const T& t)noexcept:mValue(t){}
   BaseValue(T&& t)noexcept:mValue(std::move(t)){}

   BaseValue& operator=(const T& t)noexcept{
      mValue = t;
      notice();
      return *this;
   }
   BaseValue& operator=(T&& t)noexcept{
      mValue = std::move(t);
      notice();
      return *this;
   }
   T& val()noexcept{ return mValue; }

   const T& get()const noexcept{ return mValue; }
};


template<typename Lambda, typename... Args>
class ReactiveValue{
using ValueType = std::invoke_result_t<Lambda, Args...>;
private:
   ValueType mValue;
   Observer observers[sizeof...(Args)];
public:
   ReactiveValue(Lambda computeFunc, BaseValue<Args>&... args)
   :observers{
      Observer{args, [this, computeFunc, &args...](){
         this->mValue = computeFunc(args.get()...);
      }}...
   }{
      mValue = computeFunc(args.get()...);
   }

   const ValueType& get()const noexcept{ return mValue; }
};

} // namespace minecpp

#endif // _MINECPP_TOOL_H_
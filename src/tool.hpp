#ifndef _MINECPP_TOOL_H_
#define _MINECPP_TOOL_H_

#include<vector>
#include<functional>
#include<set>

namespace minecpp
{

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

// 按照先后顺序保存一次链式调用中被改变的所有对象
inline std::vector<std::function<void(void)>*> chainCall;
inline std::set<std::function<void(void)>*> chainCallSet;

// 可观察对象
class Observable{
private:
   std::vector<std::function<void(void)>> observers;
protected:
   // abstract class
   Observable() noexcept = default;

   // 通知观察者，会防止循环调用 
   void notice(){
      for(auto& observer : observers){
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
public:
   void addObserver(const std::function<void(void)>& observer)noexcept{
      observers.push_back(observer);
   }
};

// 将对于观察者的通知封装到对数据的赋值操作中
template<typename T>
class DirtyObservable: public Observable{
private:
   T mValue;
public:
   DirtyObservable(const T&& t)noexcept:mValue(t){}
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

   const T& value()noexcept{ return mValue; }
};

// 封装数据，当调用check时检查数据距离初始化或者上次check时是否变化，如果变化则通知观察者
template<typename T>
class RefDirtyObservable: public Observable{
private:
   T mValue;
   T oldValue;
public:
   // same as dirtyobserable
   RefDirtyObservable(const T&& t)noexcept:mValue(t), oldValue(t){}
   RefDirtyObservable(T&& t)noexcept:mValue(std::move(t)), oldValue(mValue){}
   
   T& value()noexcept{return mValue;}

   void check(){
      if(oldValue != mValue){
         oldValue = mValue;
         notice();
      }
   }
};

} // namespace minecpp

#endif // _MINECPP_TOOL_H_
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

// 用于存储数据是否“脏”
// 每个observable可以产生多个关联的 observer
// 如果在一个调用链中出现了重复的 observer 就会直接结束调用
template<typename T>
class DirtyObservable{
private:
   T mValue;
   std::vector<std::function<void(void)>> observers;
   void publish() noexcept{
      
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
   DirtyObservable(T&& t)noexcept:mValue(std::move(t)){}
   
   DirtyObservable& operator=(const T& t)noexcept{
      
      mValue = t;
      publish();
      return *this;
   }
   DirtyObservable& operator=(T&& t)noexcept{
      mValue = std::move(t);
      publish();
      return *this;
   }
   const T& value()noexcept{ return mValue; }
   
   void addObserver(const std::function<void(void)>& observer)noexcept{
      observers.push_back(observer);
   }

};

} // namespace minecpp

#endif // _MINECPP_TOOL_H_
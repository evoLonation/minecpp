#ifndef _MINECPP_TOOL_H_
#define _MINECPP_TOOL_H_

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>
#include <functional>
#include <set>
#include <map>
#include "exception.hpp"

namespace minecpp
{

// 用于判断某类型是否为拥有（某个类型）的连续内存的容器

// 使用模板偏特化，在模板实例化时选择最佳的匹配，从而得到对应的bool值，达到判断是否为某个（模板）类型的目的
template<typename Container>
struct IsContiguousContainer: std::is_array<Container>{};

template<typename DataType, std::size_t N>
struct IsContiguousContainer<std::array<DataType, N>>: std::true_type{};

template<typename DataType>
struct IsContiguousContainer<std::vector<DataType>>: std::true_type{};

template<typename Container>
concept ContiguousContainer = IsContiguousContainer<Container>::value;

template<ContiguousContainer Container>
struct ContiguousData{
   using type = Container::value_type;
};
template<typename DataType, std::size_t N>
struct ContiguousData<DataType[N]>{
   using type = DataType;
};

// vector 和 std::array 都有 value_type 成员
template<ContiguousContainer Container>
using ContiguousDataType = ContiguousData<Container>::type;

// vector 和 std::array 都有 size 成员
constexpr std::size_t sizeOf(const ContiguousContainer auto& container){
   return container.size();
}

template<std::size_t N>
constexpr std::size_t sizeOf(const auto (&container)[N]){
   return N;
}

// vector 和 std::array 都有 size 成员
constexpr std::size_t sizeOfData(const ContiguousContainer auto& container){
   return container.size() * sizeof(ContiguousDataType<std::decay_t<decltype(container)>>);
}

template<std::size_t N>
constexpr std::size_t sizeOfData(const auto (&container)[N]){
   return sizeof(container);
}

constexpr auto dataAddress(const ContiguousContainer auto& container) -> ContiguousDataType<std::decay_t<decltype(container)>> const* {
   return container.data();
}

template<typename DataType, std::size_t N>
constexpr auto dataAddress(const DataType (&container)[N]) -> DataType const* {
   return container;
}

template<typename Container, typename DataType>
concept ContiguousContainerOf = ContiguousContainer<Container> && std::same_as<DataType, ContiguousDataType<Container>>;

// type list
template <typename ...P> 
struct TypePack{
   // 这种模板嵌套说明 T 可以是一个模板
   template <template <typename...> typename T> 
   using apply = T<P...>;

   template <typename ...T>
   using merge = TypePack<P..., T...>;
};

template<typename Arg1, typename Arg2>
struct InsertS;

template<typename... TypeList, typename T>
struct InsertS<TypePack<TypeList...>, T>{
   using type = TypePack<TypeList..., T>;
};

template<typename... TypeList, typename T>
struct InsertS<T, TypePack<TypeList...>>{
   using type = TypePack<T, TypeList...>;
};

template<typename Arg1, typename Arg2>
using Insert = InsertS<Arg1, Arg2>::type;

template<typename TypePack, template <typename...> typename Mapper>
struct MapToS;

template<typename... TypeList, template <typename...> typename Mapper>
struct MapToS<TypePack<TypeList...>, Mapper>{
   using type = TypePack<Mapper<TypeList>...>;
};

template<typename TypePack, template <typename...> typename Mapper>
using MapTo = MapToS<TypePack, Mapper>::type;

template<typename TypePack1, typename TypePack2>
struct MergeS;

template<typename... TypeList1, typename... TypeList2>
struct MergeS<TypePack<TypeList1...>, TypePack<TypeList2...>>{
   using type = TypePack<TypeList1..., TypeList2...>;
};

template<typename TypePack1, typename TypePack2>
using Merge = MergeS<TypePack1, TypePack2>::type;

template<typename Target, typename...Types>
consteval bool allowList(){
   return (std::same_as<Target, Types> || ...);
}

template<typename Target, typename...Types>
struct AllowList: private std::integral_constant<bool, allowList<Target, Types...>()>{
   using std::integral_constant<bool, allowList<Target, Types...>()>::value;
};


// 对于 Param&& , 如果Type为int, 其可能会被推导为 int& 或者 int&&， 如果 constant 为 true，也可被推导为 const int &
// Type 不能有 reference 和 const
template<typename Param, typename Type, bool constant = true>
concept URef = !std::is_const_v<Type> && !std::is_reference_v<Type> &&
   (std::same_as<Param&&, Type&> ||
   std::same_as<Param&&, Type&&> ||
   constant && std::same_as<Param&&, const Type&>);

template<typename Param, typename FuncType>
concept Callable = requires(Param&& a) {
   std::function<FuncType>(a);
};

template<typename ValueType> requires (!std::is_const_v<ValueType> && !std::is_reference_v<ValueType>)
class RefOrConst{
private:
   std::variant<ValueType, ValueType*> value;
public:
   RefOrConst(const ValueType& value, bool constant = false):
      value([&value, constant]() -> std::variant<ValueType, ValueType*> {
         if(constant){
            auto copy = value;
            return copy;
         }else{
            return &value;
         }
      }()){
   }
   RefOrConst(ValueType&& value):
      value(&value){}
   
   operator ValueType&(){
      return get();
   } 
   operator const ValueType&() const{
      return get();
   } 
   ValueType& get(){
      return std::visit([](auto& value) -> ValueType& {
         if constexpr(std::is_pointer_v<std::remove_reference_t<decltype(value)>>){
            return *value;
         }else{
            return value;
         }
      }, value);
   }
   const ValueType& get()const{
      return std::visit([](auto& value) -> const ValueType& {
         if constexpr(std::is_pointer_v<std::remove_reference_t<decltype(value)>>){
            return *value;
         }else{
            return value;
         }
      },value);
   }
};

// 自增的id
// id永远是唯一的
// 复制同样会增加id
template<typename T>
class IdHolder{
public:
   using IDType = int;
private:
   static IDType currentMaxId;
   IDType id;

public:

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
class IdContainer: private std::map<typename IdHolder<IdContainer<T>>::IDType, typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>>{
private:
   using WithId = IdHolder<IdContainer>;
public:
   using IDType = WithId::IDType;
private:
   using IdContainerBase = std::map<IDType, typename std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<typename std::remove_reference_t<T>>, T>>;
public:
   // 当T不是引用类型时，add函数传入左值还是右值都可，因此两个函数都开启（第一个函数不是万能引用）；当T是引用类型时，只能传入左值，T&&是左值引用
   IDType add(T&& obj){
      WithId withId;
      IdContainerBase::insert({withId.getId(), std::forward<T>(obj)});
      return withId.getId();
   }
   template <typename U = T>
   typename std::enable_if_t<!std::is_reference_v<U>, IDType> add(const U& obj) {
      WithId withId;
      IdContainerBase::insert({withId.getId(), obj});
      return withId.getId();
   }
   void remove(IDType id){
      IdContainerBase::erase(id);
   }
   // 同add函数
   template <typename U = T>
   typename std::enable_if_t<!std::is_reference_v<U>> replace(IDType id, const U& obj) {
      IdContainerBase::erase(id);
      IdContainerBase::insert({id, obj});
   }
   void replace(IDType id, T&& obj){
      IdContainerBase::erase(id);
      IdContainerBase::insert({id, std::forward<T>(obj)});
   }
   
   class Iterator: private IdContainerBase::iterator{
   public:
      using baseIterator = IdContainerBase::iterator;
      Iterator(IdContainerBase::iterator iterator): baseIterator(iterator){}

      // int 参数仅用于区分++运算符是前缀还是后缀，有int参数的是后缀
      using baseIterator::operator++;

      template <typename U = T>
      typename std::enable_if_t<!std::is_reference_v<U>, T&> operator*(){
         return baseIterator::operator->()->second;
      }
      template <typename U = T>
      typename std::enable_if_t<std::is_reference_v<U>, T> operator*(){
         return baseIterator::operator->()->second.get();
      }
      // 从 c++20 开始，会自动生成 == 对应的 != 运算符
      // 直接在类内部声明的 友元 函数
      friend bool operator==(const Iterator& ls, const Iterator& rs) {
         return static_cast<const baseIterator&>(ls) == static_cast<const baseIterator&>(rs);
      }
   };

   class ConstIterator: private IdContainerBase::const_iterator{
   public:
      using baseIterator = IdContainerBase::const_iterator;
      ConstIterator(IdContainerBase::iterator iterator): baseIterator(iterator){}

      // int 参数仅用于区分++运算符是前缀还是后缀，有int参数的是后缀
      using baseIterator::operator++;

      template <typename U = T>
      const typename std::enable_if_t<!std::is_reference_v<U>, T&> operator*() const{
         return baseIterator::operator->()->second;
      }
      template <typename U = T>
      const typename std::enable_if_t<std::is_reference_v<U>, T> operator*() const{
         return baseIterator::operator->()->second.get();
      }
      // 从 c++20 开始，会自动生成 == 对应的 != 运算符
      friend bool operator==(const ConstIterator& ls, const ConstIterator& rs) {
         return static_cast<const baseIterator&>(ls) == static_cast<const baseIterator&>(rs);
      }
   };
   Iterator begin(){
      return IdContainerBase::begin();
   }
   Iterator end(){
      return IdContainerBase::end();
   }
   ConstIterator begin() const {
      return IdContainerBase::begin();
   }
   ConstIterator end() const {
      return IdContainerBase::end();
   }
   using IdContainerBase::size;
   using IdContainerBase::find;
   using IdContainerBase::clear;

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
   RefContainer<T>::IDType id;
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

private:
   void mayReplace(){
      if constexpr (isBase()){
         container->replace(id, getMaster());
      }
   }
   auto addIn(const AutoLoader& autoLoader){
      if constexpr (isBase()) {
         return container->add(getMaster());
      }else{
         return container->add(container->find(autoLoader.id));
      }
   }
   void mayRemove(){
      if(!moved){
         container->remove(id);
      }else{
         moved = false;
      }
   }
public:

   // copy and move constructor
   AutoLoader(AutoLoader&& autoLoader): container(autoLoader.container), id(autoLoader.id){
      autoLoader.moved = true;
      mayReplace();
   }
   AutoLoader(const AutoLoader& autoLoader): container(autoLoader.container) {
      id = addIn(autoLoader);
   }
   // 移动赋值：自己的移除，并接管入参的
   AutoLoader& operator=(AutoLoader&& autoLoader) {
      mayRemove();
      container = autoLoader.container;
      id = autoLoader.id;
      autoLoader.moved = true;
      mayReplace();
      return *this;
   }
   // 拷贝赋值：自己的移除，并根据入参加载相同元素
   AutoLoader& operator=(const AutoLoader& autoLoader) {
      mayRemove();
      container = autoLoader.container;
      id = addIn(autoLoader);
      return *this;
   }
   
};

// Observable 与 Observer ，是RefContainer和AutoLoader的典型代表

template<typename... Args>
class AbstractObserver;

// 赋值操作 什么都不会发生
// 无论怎么初始化，都会获得一个空的 observer 列表
// Args 不能是引用
template<typename... Args>
class Observable {
friend class AbstractObserver<Args...>;
private:
   // observers 的状态不算 observable 的状态
   mutable RefContainer<AbstractObserver<Args...>> observers;

protected:
   Observable() = default;
   ~Observable() = default;

   // 通知观察者
   void notify(Args&... args) const;

   // 赋值时要求 被赋值者 未关联任何 Observer
   Observable& operator=(const Observable& observable) = default;
   Observable& operator=(Observable&& observable) = default;
   Observable(const Observable& observable) = default;
   Observable(Observable&& observable) = default;
};

// 拷贝语义实现同一个事件重复调用多次
// Args 不能是引用, 在参数上会表现为 Args&
template<typename... Args>
class AbstractObserver: public AutoLoader<AbstractObserver<Args...>>{
friend class Observable<Args...>;
protected:
   virtual void handle(Args&... arg) = 0;
   AbstractObserver(const Observable<Args...>& observable):
      AutoLoader<AbstractObserver<Args...>>(observable.observers){}
   AbstractObserver(Observable<Args...>&& observable) = delete;

   ~AbstractObserver() = default;

   // default move semantic
   AbstractObserver& operator=(AbstractObserver&&) = default;
   AbstractObserver(AbstractObserver&&) = default;
   // default copy semantic
   AbstractObserver& operator=(const AbstractObserver&) = default;
   AbstractObserver(const AbstractObserver&) = default;
};

template<typename... Args>
void Observable<Args...>::notify(Args&... args) const {
   for(auto& observer : observers){
      observer.handle(args...);
   }
}


// Args 在参数上会表现为 Args&
template<typename... Args>
class Observer: public AbstractObserver<Args...>{
private:
   std::function<void(Args&...)> handler;
public:
   void handle(Args&... arg) override {
      handler(arg...);
   }
   Observer(Callable<void(Args&...)> auto&& handler, const Observable<Args...>& observable)
   :  AbstractObserver<Args...>(observable), handler(std::forward<decltype(handler)>(handler)){}
   Observer(Callable<void(Args&...)> auto&& handler, Observable<Args...>&& observable) = delete;

   ~Observer() = default;

   // default move semantic
   Observer& operator=(Observer&&) = default;
   Observer(Observer&&) = default;
   // default copy semantic
   Observer& operator=(const Observer&) = default;
   Observer(const Observer&) = default;

   // 本质上是可调用的对象
   void setHandler(Callable<void(Args&...)> auto&& handler) {
      this->handler = std::forward<decltype(handler)>(handler);
   }
   
};

// 手动调用notice提示数值变化
// 拷贝、移动语义只会拷贝、移动值
template<typename T>
class ObservableValue: public Observable<const T>{
private:
   T mValue;
   mutable T oldValue;
public:
   ObservableValue() = default;
   
   template<typename U = T> requires std::equality_comparable<U>
   bool mayNotify() const {
      if(oldValue != this->mValue){
         this->notify(mValue);
         oldValue = this->mValue;
         return true;
      }
      return false;
   }
   template<typename U = T> requires (!std::equality_comparable<U>)
   bool mayNotify() const {
      this->notify(mValue);
      return true;
   }

   ObservableValue(const T& t):mValue(t), oldValue(t){}
   ObservableValue(T&& t):mValue(std::move(t)), oldValue(this->mValue){}

   ObservableValue& operator=(const T& t) {
      this->mValue = t;
      this->mayNotify();
      return *this;
   }
   ObservableValue& operator=(T&& t) {
      this->mValue = std::move(t);
      this->mayNotify();
      return *this;
   }

   const T& get() const { return mValue; }
   T& get()  { return mValue; }
   operator const T&() const {return get();}
   operator T&() {return get();}
   const T& operator*() const {return get();}
   T& operator*() {return get();}
   const T* operator->() const {return &this->get();}
   T* operator->() {return &this->get();}

   // default move semantic
   ObservableValue& operator=(ObservableValue&&) = default;
   ObservableValue(ObservableValue&&) = default;
   // default copy semantic
   ObservableValue& operator=(const ObservableValue&) = default;
   ObservableValue(const ObservableValue&) = default;
   
};

// Args 在参数上会表现为 const Args&
template<typename... Args>
class AbstractValueObserver;

template<typename... Args>
class _AbstractValueObserver{
private:
   template<typename Arg>
   class SingleObserver: public AbstractObserver<const Arg>{
   public:
      SingleObserver(const ObservableValue<Arg>& argObservable, _AbstractValueObserver& father): 
         AbstractObserver<const Arg>(argObservable), arg(&argObservable.get()), father(&father){}
      SingleObserver(ObservableValue<Arg>&& argObservable, _AbstractValueObserver& father) = delete;
      
      const Arg* arg;
      _AbstractValueObserver* father;
      void handle(const Arg& arg) override {
         father->valueUpdated();
      }
   };
   std::tuple<SingleObserver<Args>...> observers;
protected:
   void updateFatherPointer(){
      // 使用 std::apply 实现 tuple 的遍历执行
      std::apply([this](auto&... args) {((args.father = this), ...);}, observers);
   }

   virtual void handle(const Args&...) = 0;
   void handleObservers(SingleObserver<Args>&...  observers){
      handle(*observers.arg...);
   }
   // 子类自行决定构造的何时更新值
   void valueUpdated(){
      // std::apply 用于将tuple展开作为参数传入函数
      // 调用类的成员函数：在前面加&Class, 并需要 bind_front
      std::apply(std::bind_front(&_AbstractValueObserver::handleObservers, this), observers);
   }
public:
   _AbstractValueObserver(const ObservableValue<Args>&... args):
      observers{SingleObserver<Args>(args, *this)...}{}
   // 禁止传入右值
   _AbstractValueObserver(ObservableValue<Args>&&... args) = delete;

   _AbstractValueObserver(_AbstractValueObserver&& observer) = default;
   _AbstractValueObserver(const _AbstractValueObserver& observer) = default;
   _AbstractValueObserver& operator=(_AbstractValueObserver&& observer) = default;
   _AbstractValueObserver& operator=(const _AbstractValueObserver& observer) = default;
};


template<typename... Args>
class AbstractValueObserver: public _AbstractValueObserver<Args...>{
public:
   using _AbstractValueObserver<Args...>::_AbstractValueObserver;

   // copy semantic
   AbstractValueObserver& operator=(const AbstractValueObserver& value) {
      _AbstractValueObserver<Args...>::operator=(value);
      _AbstractValueObserver<Args...>::updateFatherPointer();
      return *this;
   }
   AbstractValueObserver(const AbstractValueObserver& value): _AbstractValueObserver<Args...>(value){
      _AbstractValueObserver<Args...>::updateFatherPointer();
   }
   // move semantic
   AbstractValueObserver& operator=(AbstractValueObserver&& value) {
      _AbstractValueObserver<Args...>::operator=(std::move(value));
      _AbstractValueObserver<Args...>::updateFatherPointer();
      return *this;
   }
   AbstractValueObserver(AbstractValueObserver&& value): _AbstractValueObserver<Args...>(std::move(value)) {
      _AbstractValueObserver<Args...>::updateFatherPointer();
   }
};

// getTarget() 待实现
// 拷贝操作和移动操作后，响应式关系仍然存在，即target仍然观察args的变化
// Args 在参数上会表现为 const Args&
template<typename Derived, typename T, typename... Args>
class BaseReactive: public AbstractValueObserver<Args...>{
private:
   std::function<T(const Args&...)> computeFunc;
   ObservableValue<T>& getTarget(){
      return static_cast<Derived&>(*this).getTarget();
   }
protected:
   void handle(const Args&... args) override {
      getTarget() = computeFunc(args...);
   }
public:
   BaseReactive(Callable<T(const Args&...)> auto&& computeFunc, const ObservableValue<Args>&... args):
      AbstractValueObserver<Args...>(args...),
      computeFunc(std::forward<decltype(computeFunc)>(computeFunc)){}
   // 禁止传入右值
   BaseReactive(Callable<T(const Args&...)> auto&& computeFunc, ObservableValue<Args>&&... args) = delete;

   BaseReactive(BaseReactive&& observer) = default;
   BaseReactive(const BaseReactive& observer) = default;
   BaseReactive& operator=(BaseReactive&& observer) = default;
   BaseReactive& operator=(const BaseReactive& observer) = default;
};

template<typename T, typename... Args>
class ReactiveValue: public ObservableValue<T>, public BaseReactive<ReactiveValue<T, Args...>, T, Args...>{
   using Base = BaseReactive<ReactiveValue<T, Args...>, T, Args...>;
   friend Base;
public:
   ReactiveValue(Callable<T(const Args&...)> auto&& computeFunc, const ObservableValue<Args>&... args): 
      Base(std::forward<decltype(computeFunc)>(computeFunc), args...) 
   {
      this->valueUpdated();
   }
   // 禁止传入右值
   ReactiveValue(Callable<T(const Args&...)> auto&& computeFunc, ObservableValue<Args>&&... args) = delete;

   ObservableValue<T>& getTarget() {
      return static_cast<ObservableValue<T>&>(*this);
   }
};

template<typename T, typename... Args>
class ReactiveBinder: BaseReactive<ReactiveBinder<T, Args...>, T, Args...>{
   using Base = BaseReactive<ReactiveBinder<T, Args...>, T, Args...>;
   friend Base;
private:
   ObservableValue<T>* target;
public:
   ReactiveBinder(Callable<T(const Args&...)> auto&& computeFunc, ObservableValue<T>& target, const ObservableValue<Args>&... args):
      Base(std::forward<decltype(computeFunc)>(computeFunc), args...),
      target(&target)
   {
      this->valueUpdated();
   }
   // 禁止传入右值
   ReactiveBinder(Callable<T(const Args&...)> auto&& computeFunc, ObservableValue<T>& target, ObservableValue<Args>&&... args) = delete;

   // deleted copy semantic
   // 一个响应式对应一个目标，没必要复制，复制只会浪费计算资源，观察多次
   ReactiveBinder& operator=(const ReactiveBinder&) = delete;
   ReactiveBinder(const ReactiveBinder&) = delete;
   
   ObservableValue<T>& getTarget() {
      return *target;
   }
};

template<typename... Args, std::invocable<Args...> Callable>
ReactiveValue<std::invoke_result_t<Callable, Args...>, Args...> makeReactiveValue(Callable&& computeFunc, ObservableValue<Args>&... args){
   return ReactiveValue<std::invoke_result_t<Callable, Args...>,  Args...>(std::forward<Callable>(computeFunc), args...);
}

template<typename... Args, std::invocable<Args...> Callable>
ReactiveBinder<std::invoke_result_t<Callable, Args...>, Args...> makeReactiveBinder(Callable&& computeFunc, ObservableValue<std::invoke_result_t<Callable, Args...>>& target, ObservableValue<Args>&... args){
   return ReactiveBinder<std::invoke_result_t<Callable, Args...>,  Args...>(std::forward<Callable>(computeFunc), target, args...);
}




} // namespace minecpp

#endif // _MINECPP_TOOL_H_
#include <objc/runtime.h>

template <typename Superclass>
struct ObjCClass
{
  ObjCClass() : _cls(objc_allocateClassPair([Superclass class], randomName().c_str(), 0))
  {
    assert(_cls != nullptr);
  }

  ~ObjCClass() noexcept
  {
    if (_cls != nullptr) objc_disposeClassPair(_cls);
  }

  Superclass *create() const
  {
    return class_createInstance(_cls, 0);
  }

  template <typename Type>
  void addIvar(const char *name)
  {
    assert(class_addIvar(_cls, name, sizeof(Type), (uint8_t)rint(log2(sizeof(Type))), @encode(Type)));
  }

  template <typename Fn>
  void addMethod(SEL selector, Fn callbackFn)
  {
    addMethod(selector, toFnPtr(callbackFn));
  }

  template <typename Result, typename... Args>
  void addMethod(SEL selector, Result (*callbackFn)(id, SEL, Args...))
  {
    std::string name;

    name += @encode(Result);
    name += @encode(id);
    name += @encode(SEL);

    const char *args[] = { @encode(Args)... };
    for (const char *enc : args) name += enc;

    assert(class_addMethod(_cls, selector, (IMP)callbackFn, name.c_str()));
  }

  void addProtocol(Protocol *protocol)
  {
    assert(class_addProtocol(_cls, protocol));
  }

 private:
  static std::string randomName()
  {
    auto now = std::chrono::high_resolution_clock::now();
    auto time = now.time_since_epoch().count();
    auto random = std::rand();
    std::string name = "ClapAsVst3_";
    name += std::to_string(time);
    name += "_";
    name += std::to_string(random);
    return name;
  }

  Class _cls;
};

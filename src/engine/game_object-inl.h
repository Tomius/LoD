// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_INL_H_
#define ENGINE_GAME_OBJECT_INL_H_

#include "./game_object.h"

namespace engine {

template<typename T, typename... Args>
T* GameObject::addComponent(Args&&... args) {
  static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

  try {
    T *obj = new T(this, std::forward<Args>(args)...);
    components_.push_back(std::unique_ptr<GameObject>(obj));
    components_just_enabled_.push_back(obj);

    return obj;
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return nullptr;
  }
}

}  // namespace engine

#endif

// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_INL_H_
#define ENGINE_GAME_OBJECT_INL_H_

#include <iostream>
#include "./game_object.h"

namespace engine {

template<typename Transform_t>
GameObject::GameObject(GameObject* parent, const Transform_t& transform)
    : scene_(parent ? parent->scene_ : nullptr), parent_(parent)
    , transform_(new Transform_t{transform})
    , layer_(0), group_(0), enabled_(true) {
  if (parent) { transform_->set_parent(parent_->transform()); }
  sorted_components_.insert(this);
}

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

// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_GAME_OBJECT_INL_H_
#define ENGINE_GAME_OBJECT_INL_H_

#include <cassert>
#include <iostream>
#include "./game_object.h"

namespace engine {

template<typename Transform_t>
GameObject::GameObject(GameObject* parent, const Transform_t& transform)
    : scene_(parent ? parent->scene_ : nullptr), parent_(parent)
    , transform_(new Transform_t{transform})
    , enabled_(true) {
  assert(parent != this);
  if (parent) { transform_->set_parent(parent_->transform()); }
}

template<typename T, typename... Args>
T* GameObject::addComponent(Args&&... args) {
  static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

  try {
    T *obj = new T(this, std::forward<Args>(args)...);
    components_.push_back(std::unique_ptr<GameObject>(obj));
    // make sure that the object is aware of the screen's size
    obj->InitScreenSize();

    return obj;
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return nullptr;
  }
}

template<typename T>
T* GameObject::FindComponent(const GameObject* go) {
  if (!go) { return nullptr; }

  for (auto& comp_ptr : go->components_) {
    GameObject* comp = comp_ptr.get();
    T* t = dynamic_cast<T*>(comp);
    if (t) {
      return t;
    } else {
      t = FindComponent<T>(comp);
      if (t) { return t; }
    }
  }
  return nullptr;
}

template<typename T>
void GameObject::FindComponents(const GameObject* go, std::vector<T*> *found) {
  if (!go) { return; }

  for (auto& comp_ptr : go->components_) {
    GameObject* comp = comp_ptr.get();
    T* t = dynamic_cast<T*>(comp);
    if (t) {
      found->push_back(t);
    }
    FindComponent<T>(comp);
  }
  return;
}

template<typename T>
std::vector<T*> GameObject::findComponents() const {
  std::vector<T*> found;
  FindComponents<T>(this, &found);
  return found;
}

inline std::unique_ptr<GameObject> GameObject::removeComponent(GameObject* component_to_remove) {
  if (component_to_remove == nullptr) {
    return nullptr;
  }

  for (auto iter = components_.begin();
       iter != components_.end(); ++iter) {
    if (iter->get() == component_to_remove) {
      auto ptr = iter->release();
      ptr->scene_ = nullptr;
      ptr->set_parent(nullptr);
      components_.erase(iter);
      return std::unique_ptr<GameObject>{ptr};
    }
  }

  return nullptr;
}

inline void GameObject::ClearComponents() {
  components_.clear();
}

}  // namespace engine

#endif

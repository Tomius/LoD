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
    , group_(0), enabled_(true) {
  if (parent) { transform_->set_parent(parent_->transform()); }
  sorted_components_.insert(this);
}

template<typename T, typename... Args>
T* GameObject::addComponent(Args&&... args) {
  static_assert(std::is_base_of<GameObject, T>::value, "Unknown type");

  try {
    T *obj = new T(this, std::forward<Args>(args)...);
    obj->uid_ = NextUid();
    components_.push_back(std::unique_ptr<GameObject>(obj));
    components_just_enabled_.push_back(obj);

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

inline bool GameObject::stealComponent(GameObject* go) {
  if (!go) { return false; }
  GameObject* parent = go->parent();
  if (!parent) {return false; }

  for (auto iter = parent->components_.begin();
       iter != parent->components_.end(); ++iter) {
    GameObject* comp = iter->get();
    if (comp == go) {
      components_.push_back(std::unique_ptr<GameObject>(iter->release()));
      components_just_enabled_.push_back(comp);
      parent->components_just_disabled_.push_back(comp);
      // The iter->release() leaves a nullptr in the parent->components_
      // that should be removed, as it decrases performance
      parent->removeComponent(nullptr);
      comp->parent_ = this;
      comp->transform_->set_parent(transform_.get());
      comp->scene_ = scene_;
      comp->uid_ = NextUid();
      return true;
    }
  }
  return false;
}

inline int GameObject::NextUid() {
  static int uid = 0;
  return uid++;
}

inline void GameObject::removeComponent(GameObject* component_to_remove) {
  if (component_to_remove == nullptr) { return; }
  components_just_disabled_.push_back(component_to_remove);
  remove_predicate_.components_.insert(component_to_remove);
}

template <typename T>
void GameObject::removeComponents(T begin, T end) {
  components_just_disabled_.insert(components_just_disabled_.end(), begin, end);
  remove_predicate_.components_.insert(begin, end);
}

}  // namespace engine

#endif

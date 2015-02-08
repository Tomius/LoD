// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_RIGID_BODY_H_
#define ENGINE_RIGID_BODY_H_

#include "./transform.h"
#include "./behaviour.h"
#include "./height_map_interface.h"

namespace engine {

class RigidBody : public Behaviour {
 public:
  RigidBody(GameObject* parent, Transform* target,
            const HeightMapInterface& height_map,
            double starting_height = NAN)
      : Behaviour(parent)
      , target_(target)
      , height_map_(height_map) {
    Transform* targets_parent = target_->parent();
    transform()->set_parent(targets_parent);
    target_->set_parent(transform());

    if (std::isnan(starting_height)) {
      auto pos = target_->pos();
      last_height_ = height_map_.heightAt(pos.x, pos.z);
    } else {
      last_height_ = starting_height;
    }
  }

 private:
  Transform* target_;
  double last_height_;
  const HeightMapInterface& height_map_;

  virtual void update() override {
    auto pos = target_->pos();
    double new_height = height_map_.heightAt(pos.x, pos.z);
    float diff = new_height - last_height_;
    last_height_ = new_height;

    glm::vec3 my_local_pos = transform()->local_pos();
    my_local_pos.y += diff;
    transform()->set_local_pos(my_local_pos);

    glm::vec3 target_local_pos = target_->local_pos();
    target_local_pos.y -= diff;
    target_->set_local_pos(target_local_pos);
  }
};

}  // namespace engine

#endif

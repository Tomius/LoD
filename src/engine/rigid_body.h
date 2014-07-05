// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_RIGID_BODY_H_
#define ENGINE_RIGID_BODY_H_

#include "./transform.h"
#include "./height_map_interface.h"

namespace engine {

class RigidBody {
  Transform transform_;
  Transform& local_transform_;
  double last_height_;

  const HeightMapInterface& height_map_;

public:
  RigidBody(Transform& local_transform,
            const HeightMapInterface& height_map,
            double starting_height = NAN)
              : local_transform_(local_transform)
              , height_map_(height_map) {
    Transform* parent = local_transform.parent();
    transform_.set_parent(parent);
    local_transform.set_parent(&transform_);

    if (std::isnan(starting_height)) {
      auto pos = local_transform_.pos();
      last_height_ = height_map_.heightAt(pos.x, pos.z);
    } else {
      last_height_ = starting_height;
    }

  }

  void update() {
    auto pos = local_transform_.pos();
    double new_height = height_map_.heightAt(pos.x, pos.z);
    float diff = new_height - last_height_;
    last_height_ = new_height;

    transform_.local_pos().y += diff;
    local_transform_.local_pos().y -= diff;
  }

};

}

#endif

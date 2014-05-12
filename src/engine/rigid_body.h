// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_RIGID_BODY_H_
#define ENGINE_RIGID_BODY_H_

#include "transform.h"

namespace engine {

class RigidBody {
  Transform transform_;
  Transform& local_transform_;
  double last_height_;

  std::function<double(double, double)> getTerrainHeight_;

public:
  using CallBack = const std::function<double(double, double)>&;
  RigidBody(Transform& local_transform,
            CallBack getTerrainHeight,
            double starting_height = NAN)
              : local_transform_(local_transform)
              , getTerrainHeight_(getTerrainHeight) {
    Transform* parent = local_transform_.getParent();
    if (parent) {
      parent->removeChild(local_transform_);
      parent->addChild(transform_);
    }
    transform_.addChild(local_transform);

    if (std::isnan(starting_height)) {
      auto pos = local_transform_.pos();
      last_height_ = getTerrainHeight(pos.x, pos.z);
    } else {
      last_height_ = starting_height;
    }

  }

  void update() {
    auto pos = local_transform_.pos();
    double new_height = getTerrainHeight_(pos.x, pos.z);
    float diff = new_height - last_height_;
    last_height_ = new_height;

    transform_.localPos().y += diff;
    local_transform_.localPos().y -= diff;
  }

};

}

#endif

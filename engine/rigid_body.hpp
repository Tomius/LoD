#ifndef ENGINE_RIGID_BODY_HPP_
#define ENGINE_RIGID_BODY_HPP_

#include "transform.hpp"

namespace engine {

class RigidBody {
  Transform transform_;
  Transform& local_transform_;

  std::function<double(double, double)> getTerrainHeight_;

public:
  RigidBody(Transform& local_transform,
            const std::function<double(double, double)>& getTerrainHeight)
              : local_transform_(local_transform)
              , getTerrainHeight_(getTerrainHeight) {
    Transform* parent = local_transform_.getParent();
    if(parent) {
      parent->removeChild(local_transform_);
      parent->addChild(transform_);
    }
    transform_.addChild(local_transform);
  }

  void update() {
    auto pos = local_transform_.pos_proxy();
    double new_height = getTerrainHeight_(pos.x, pos.z);
    float diff = new_height - pos.y;
    transform_.localPos().y += diff;
    pos.y -= diff;
  }

};

}

#endif

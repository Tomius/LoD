// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_COLLISION_BOUNDING_BOX_H_
#define ENGINE_COLLISION_BOUNDING_BOX_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "../misc.h"
#include "./frustum.h"

namespace engine {

class BoundingBox {
  glm::vec3 mins_;
  glm::vec3 maxes_;
 public:
  BoundingBox() = default;

  BoundingBox(const glm::vec3& mins, const glm::vec3& maxes)
      : mins_(mins), maxes_(maxes) {}

  glm::vec3 mins() const { return mins_; }
  glm::vec3 maxes() const { return maxes_; }
  glm::vec3 center() const { return (maxes_+mins_) / 2.0f; }
  glm::vec3 extent() const { return maxes_-mins_; }

  bool collidesWithSphere(const glm::vec3& center, float radius) const {
    float dmin = 0;
    for (int i = 0; i < 3; ++i) {
      if (center[i] < mins_[i]) {
        dmin += sqr(center[i] - mins_[i]);
      } else if (center[i] > maxes_[i]) {
        dmin += sqr(center[i] - maxes_[i]);
      }
    }
    return dmin <= sqr(radius);
  }

  bool collidesWithFrustum(const Frustum& frustum) const {
    glm::vec3 center = this->center();
    glm::vec3 extent = this->extent();

    for(int i = 0; i < 6; ++i) {
      const Plane& plane = frustum.planes[i];

      float d = glm::dot(center, plane.normal);
      float r = glm::dot(extent, glm::abs(plane.normal));

      if(d + r < -plane.dist) {
        return false;
      }
    }
    return true;
  }
};

}


#endif

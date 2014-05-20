// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_COLLISION_PLANE_H_
#define ENGINE_COLLISION_PLANE_H_

#include "../../oglwrap/glm/glm/glm.hpp"
#include "../misc.h"

struct Plane {
  glm::vec3 normal;
  float dist;
  Plane(float nx, float ny, float nz, float dist)
      : Plane(glm::vec3(nx, ny, nz), dist) { }

  Plane(const glm::vec3& normal, float dist) {
    float l = glm::length(normal);
    this->normal = normal / l;
    this->dist = dist / l;
  }
};

#endif

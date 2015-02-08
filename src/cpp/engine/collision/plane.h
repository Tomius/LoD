// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_COLLISION_PLANE_H_
#define ENGINE_COLLISION_PLANE_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct Plane {
  glm::vec3 normal;
  float dist;
  Plane() = default;
  Plane(float nx, float ny, float nz, float dist)
      : normal(nx, ny, nz), dist(dist) { }
  Plane(const glm::vec3& normal, float dist)
      : normal(normal), dist(dist) { }

  void normalize() {
    float l = glm::length(normal);
    normal /= l;
    dist /= l;
  }
};

#endif

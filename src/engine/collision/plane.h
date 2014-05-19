// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_COLLISION_PLANE_H_
#define ENGINE_COLLISION_PLANE_H_

#include "../../oglwrap/glm/glm/glm.hpp"
#include "../misc.h"

struct Plane {
  glm::vec3 normal;
  float dist;
};

#endif

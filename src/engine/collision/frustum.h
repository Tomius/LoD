// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_COLLISION_FRUSTUM_H_
#define ENGINE_COLLISION_FRUSTUM_H_

#include <array>
#include "../../oglwrap/glm/glm/glm.hpp"
#include "plane.h"
#include "../misc.h"

struct Frustum {
  std::array<Plane, 6> planes; // left, right, top, down, near, far
};

#endif

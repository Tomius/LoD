// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_COLLISION_CONE_H_
#define ENGINE_COLLISION_CONE_H_

#define GLM_FORCE_RADIANS
#include "../../oglwrap/glm/glm/glm.hpp"

struct Cone {
  /*
       root
        x
       /|\ <- angle
      / | \
     /     \
       dir
   */
  glm::vec3 root, dir;
  float cos_angle, end_dist;

  Cone(const glm::vec3& root, const glm::vec3& dir,
       float cos_angle, float end_dist = 1.0f/0.0f)
    : root(root), dir(dir), cos_angle(cos_angle), end_dist(end_dist) { }

  static Cone ShadowCone(const glm::vec3& light_pos, const glm::vec4& bsphere,
                         float max_dist) {
    glm::vec3 diff = light_pos - glm::vec3(bsphere);
    float len = glm::length(diff);
    glm::vec3 dir = diff / len;
    float cos_angle = cos(atan(bsphere.w / len));

    return Cone(light_pos, dir, cos_angle, max_dist);
  }

  bool intersects(const Cone& cone) {

  }
};

#endif

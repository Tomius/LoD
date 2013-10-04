#include "hair.hpp"

Hair::HairSegment::HairSegment(BasicHairSegment* _parent,
                               oglwrap::ExternalBone& ebone)
    : bone(ebone)
    , parent(_parent) {

  glm::vec3 parent_pos = parent ? parent->pos : glm::vec3();

  pos = parent_pos + glm::vec3(bone.offset * glm::vec4(0, 0, 0, 1));
  default_pos = pos;
  length = glm::length(pos - parent_pos);

  for(size_t i = 0; i != bone.child.size(); ++i) {
    child.push_back(HairSegment(this, bone.child[i]));
  }
}

Hair::Hair(const oglwrap::ExternalBoneTree& root_ebone,
           const oglwrap::CharacterMovement& charmove)
     : root_(root_ebone)
     , charmove_(charmove) {

  for(size_t i = 0; i != root_.bone.child.size(); ++i) {
    root_.child.push_back(HairSegment(&root_, root_.bone.child[i]));
  }
}

void Hair::update(float time, float gravity) {
  static float last_time;
  float dt = time - last_time;
  last_time = time;

  glm::mat4 model_matrix = charmove_.getModelMatrix();
  inverse_model_matrix_ = glm::inverse(model_matrix);

  root_.pos = glm::vec3(
      model_matrix * root_.bone.transform * glm::vec4(0, 0, 0, 1)
  );

  for(size_t i = 0; i != root_.child.size(); ++i) {
    updateNode(root_.child[i], dt, gravity);
  }
}

void Hair::updateNode(HairSegment& node, float delta_t, float gravity) {
  /*
     A segments of the hair can be treated a pendulum.
     We need only to know it's effective force (acceleration), to simulate its movement.
     And in fact its quite simple, only two forces apply to it: the "rope-force" and the
     gravity.

      ------x-----------
             \
              ^ rope-force (r)
               \
                o
                |
    gravity (g) V

    Note that we don't have to use the mass in the calculations,
    actually we only need to count with the accelerations.

    The signs:
        g - Gravitational acceleration
        r - The acceleration caused by the force that invokes inside the bone.
        dr - The direction of 'r' (unit vector)
        gr - The gravity's r-parallel component
        R - The length of the bone.
      ----------------------------------------------
        v(t) - tangential velocity vector
        v(r) - radial velocity
        a(t) - tangential acceleration
        a(r) - radial acceleration
      ----------------------------------------------
        a(cp) - centripetal acceleration

    The gravity's r-parallel component (gr) can be counted as:
        gr = dot(dr, g) * dr
    We know that it will have a pure circular motion, because the
    length of the bone won't change, so a(r) will equal a(cp):
        a(r) = r + gr
    But we know that
        a(cp) = (length(v(t)) ^ 2 / R) * dr
        a(cp) = a(r)
    so it gives us the equation:
        (length(v(t)) ^ 2 / R) * dr = r + dot(dr, g) * dr
    And we only don't know 'r' from here, so:
        r = (dot(dr, g) - length(v(t)) ^ 2 / R) * dr
    And once we got that, we can have the sum of the accelerations.
        sum = r + g

  */

  if(node.length > 0) {
//    glm::vec3 g = glm::vec3(0, -gravity, 0);
//    glm::vec3 dr = glm::normalize(node.pos - node.parent->pos);
//
//    // The tangential part of the velocity can be counted
//    // as the whole velocity minus its radial part.
//    float vt = glm::length(node.velocity - glm::dot(dr, node.velocity) * dr);
//    float R = node.length;
//
//    glm::vec3 r = (glm::dot(dr, g) - vt*vt / R) * dr;
//    glm::vec3 effective_acceleration = r + g;
//
//    effective_acceleration *= (1.0f - kHairSimulationDrag);
//
//    node.velocity += effective_acceleration * delta_t;
//    node.pos += node.velocity * delta_t;
//
//    glm::vec3 offset_vector = glm::mat3(inverse_model_matrix_) * (node.pos - node.default_pos);
//    node.bone.transform = glm::translate(glm::mat4(), offset_vector);
  }
    node.bone.transform = glm::mat4();

  for(size_t i = 0; i != node.child.size(); ++i) {
    updateNode(node.child[i], delta_t, gravity);
  }
}

#include "hair.hpp"

Hair::HairSegment::HairSegment(HairSegment* _parent, const aiNode* _node, glm::vec3 parent_pos) {
  node = _node;
  glm::mat4 transformation = oglwrap::convertMatrix(node->mTransformation);
  pos = glm::vec3(transformation * glm::vec4(parent_pos, 1));
  length = glm::length(pos - parent_pos);

  parent = _parent;
  childs = new HairSegment*[node->mNumChildren];
}

//Hair::Hair(const std::string& name,
//           oglwrap::SkinningData* skin_data,
//           const oglwrap::CharacterMovement& charmove)
//     : skinning_data_(*skin_data)
//     , charmove(charmove_) {
//
//  const aiNode* root_node = nullptr;
//
//  root_ = new HairSegment(nullptr, root_node, glm::vec3());
//  for(size_t i = 0; i != root_node->mNumChildren; ++i) {
//    InitNode(root_, i);
//  }
//}

void Hair::InitNode(HairSegment* parent, int id) {
  HairSegment* segment = new HairSegment(parent, parent->node->mChildren[id], parent->pos);
  parent->childs[id] = segment;
  for(size_t i = 0; i != segment->node->mNumChildren; ++i) {
    InitNode(segment, i);
  }
}

void Hair::Update(glm::vec3 new_pos, float time, float gravity) {
  static float last_time;
  float dt = time - last_time;
  last_time = time;

  root_->pos = new_pos;
  root_->velocity = (new_pos - root_->pos) / dt; // v = ds / dt

  for(size_t i = 0; i != root_->node->mNumChildren; ++i) {
    UpdateTree(root_->childs[i], dt, gravity);
  }
}

void Hair::UpdateTree(HairSegment* node, float dt, float gravity) {
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
        v(t) - tangential velocity
        v(r) - radial velocity
        a(t) - tangential acceleration
        a(r) - radial acceleration
      ----------------------------------------------
        a(cp) - centripetal acceleration

    The gravity's r-parallel component (gr) can be counted as:
        gr = dot(dr, g) * -dr
    We know that it will have a pure circular motion, because the
    length of the bone won't change, so a(r) will equal a(cp):
        a(r) = r + gr
    But we know that
        a(cp) = v(t)^2 / R
        a(cp) = a(r)
    so it gives us the equation:
        v(t)^2 / R = r + dot(dr, g) * -dr
    And we only don't know 'r' from here, so:
        r = dot(dr, g) * -dr - v(t)^2 / R
    And once we got that, we can have the sum of the accelerations.
        sum = r + g

  */

  glm::vec3 g = glm::vec3(0, -gravity, 0);
  glm::vec3 dr = glm::normalize(node->pos - node->parent->pos);
  glm::vec3 r = glm::dot(dr, g) * dr;

  const float drag = 0.1f;
  glm::vec3 effective_acceleration = r + g;
  effective_acceleration *= (1.0f - drag);

  node->velocity += effective_acceleration * dt;
  node->pos += node->velocity * dt;
}

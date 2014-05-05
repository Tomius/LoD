/** @file transform.hpp
    @brief Implements Transformation
*/

#include <vector>
#include "oglwrap/oglwrap.hpp"

#ifndef ENGINE_TRANSFORM_HPP_
#define ENGINE_TRANSFORM_HPP_

class Transform {
  Transform* parent_;
  std::vector<Transform*> children_;
  glm::vec3 pos_, scale_;
  glm::quat rot_;

public:
  Transform(Transform* parent = nullptr)
    : parent_(parent)
    , scale_(1, 1, 1)
  { }

  void addChild(Transform *t) {
    if(t) {
      t->parent_ = this;
      children_.push_back(t);
    }
  }

  class WorldSpacePositionProxy : public glm::vec3 {
    Transform& tf;
  public:
    WorldSpacePositionProxy(Transform& tf) : glm::vec3(tf.pos()), tf(tf) { }
    ~WorldSpacePositionProxy() { tf.pos(*this); }
  };

  WorldSpacePositionProxy pos_proxy() {
    return WorldSpacePositionProxy(*this);
  }

  const glm::vec3 pos() const {
    if(parent_) {
      return glm::vec3(parent_->localToWorldMatrix() * glm::vec4(pos_, 1));
    } else {
      return pos_;
    }
  }

  void pos(const glm::vec3& new_pos) {
    pos_ = parent_ ? new_pos - parent_->pos() : new_pos;
  }

  glm::vec3& localPos() {
    return pos_;
  }

  const glm::vec3& localPos() const {
    return pos_;
  }

  void localPos(const glm::vec3& new_pos) {
    pos_ = new_pos;
  }

  class WorldSpaceScaleProxy : public glm::vec3 {
    Transform& tf;
  public:
    WorldSpaceScaleProxy(Transform& tf) : glm::vec3(tf.scale()), tf(tf) { }
    ~WorldSpaceScaleProxy() { tf.scale(*this); }
  };

  WorldSpaceScaleProxy scale_proxy() {
    return WorldSpaceScaleProxy(*this);
  }

  const glm::vec3 scale() const {
    if(parent_) {
      return glm::mat3(parent_->localToWorldMatrix()) * scale_;
    } else {
      return scale_;
    }
  }

  void scale(const glm::vec3& new_scale) {
    scale_ = new_scale / parent_->scale();
  }

  glm::vec3& localScale() {
    return scale_;
  }

  const glm::vec3& localScale() const {
    return scale_;
  }

  void localScale(const glm::vec3& new_scale) {
    scale_ = new_scale;
  }

  class WorldSpaceRotationProxy : public glm::quat {
    Transform& tf;
  public:
    WorldSpaceRotationProxy(Transform& tf) : glm::quat(tf.rot()), tf(tf) { }
    ~WorldSpaceRotationProxy() { tf.rot(*this); }
  };

  const glm::quat rot() {
    if(parent_) {
      return parent_->rot() * rot_;
    } else {
      return rot_;
    }
  }

  void rot(const glm::quat& new_rot) {
    if(parent_) {
      rot_ = glm::inverse(parent_->rot()) * new_rot;
    } else {
      rot_ = new_rot;
    }
  }

  glm::quat& localRot() {
    return rot_;
  }

  const glm::quat& localRot() const {
    return rot_;
  }

  glm::vec3 forward() {
    return rot() * glm::vec3(0, 0, 1);
  }

  glm::vec3 up() {
    return rot() * glm::vec3(0, 1, 0);
  }

  glm::vec3 right() {
    return rot() * glm::vec3(1, 0, 0);
  }

  operator glm::mat4() {
    return localToWorldMatrix();
  }

  glm::mat4 worldToLocalMatrix() {
    return glm::inverse(localToWorldMatrix());
  }

  glm::mat4 localToWorldMatrix() {
    if(parent_) {
      return parent_->localToWorldMatrix() * glm::scale(
        glm::mat4_cast(rot_) * glm::translate(glm::mat4(), pos_), scale_
      );
    } else {
      return glm::scale(
        glm::mat4_cast(rot_) * glm::translate(glm::mat4(), pos_), scale_
      );
    }
  }
};

#endif

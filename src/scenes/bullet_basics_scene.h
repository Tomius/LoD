// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SCENES_BULLET_BASICS_SCENE_H_
#define LOD_SCENES_BULLET_BASICS_SCENE_H_

#include <vector>
#include <bullet/btBulletDynamicsCommon.h>
#include "../engine/misc.h"
#include "../engine/scene.h"
#include "../engine/camera.h"
#include "../engine/behaviour.h"
#include "../engine/shapes/cube_mesh.h"

using engine::shapes::CubeMesh;

class BulletRigidBody : public engine::Behaviour {
 public:
  template <typename Shape_t, typename... Args>
  explicit BulletRigidBody(engine::Scene* scene, float mass, Args&&... args)
      : engine::Behaviour(scene) {
    shape_ = engine::make_unique<Shape_t>(std::forward<Args>(args)...);
    btVector3 inertia(0, 0, 0);
    if (mass != 0.0f) {
      shape_->calculateLocalInertia(mass, inertia);
    }
    motion_state_ = engine::make_unique<btDefaultMotionState>();
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion_state_.get(),
                                                  shape_.get(), inertia);
    rigid_body_ = engine::make_unique<btRigidBody>(info);
    scene_->world()->addRigidBody(rigid_body_.get());
  }

 private:
  std::unique_ptr<btMotionState> motion_state_;
  std::unique_ptr<btRigidBody> rigid_body_;
  std::unique_ptr<btCollisionShape> shape_;
  virtual void update() override {
    btTransform t;
    rigid_body_->getMotionState()->getWorldTransform(t);
    const btVector3& o = t.getOrigin();
    parent_->transform.set_pos(glm::vec3(o.x(), o.y(), o.z()));
    const btVector3& axis = t.getRotation().getAxis();
    const btScalar& w = t.getRotation().getW();
    parent_->transform.set_rot(glm::quat(w, axis.x(), axis.y(), axis.z()));
  }
};

struct RigidBodyDeleter {
  void operator()(btRigidBody *ptr) const {
    delete ptr->getMotionState();
    delete ptr->getCollisionShape();
    delete ptr;
  }
};

using RigidBodyPtr = std::unique_ptr<btRigidBody, RigidBodyDeleter>;

class BulletBasicsScene : public engine::Scene {
  std::unique_ptr<btCollisionConfiguration> collision_config_;
  std::unique_ptr<btDispatcher> dispatcher_;
  // should Bullet examine every object, or just what close to each other
  std::unique_ptr<btBroadphaseInterface> broadphase_;
  // solve collisions, apply forces, impulses
  std::unique_ptr<btConstraintSolver> solver_;

  std::vector<RigidBodyPtr> rbodies_;

  void addSphere() {
    const float radius = 1.0f;
    const float mass = 1.0f;
    btTransform transform;
    transform.setIdentity();
    auto cam = camera();
    glm::vec3 pos = cam->pos() + 2.0f*cam->forward();
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    btSphereShape* sphere = new btSphereShape(radius);
    btVector3 inertia(0, 0, 0);
    if (mass != 0.0f) {
      sphere->calculateLocalInertia(mass, inertia);
    }
    btMotionState* motion = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
    btRigidBody* rbody = new btRigidBody(info);
    rbodies_.push_back(RigidBodyPtr(rbody));
    world_->addRigidBody(rbody);
    auto plane_mesh = addGameObject<CubeMesh>(glm::vec3(1.0, 0.0, 0.0));
    plane_mesh->transform.set_local_pos(pos);
  }

 public:
  BulletBasicsScene()
      : collision_config_(new btDefaultCollisionConfiguration())
      , dispatcher_(new btCollisionDispatcher(collision_config_.get()))
      // Dynamic bounding volume tree broadphase
      // Alternatively I could use btAxisSweep3 for finite bound worlds.
      , broadphase_(new btDbvtBroadphase())
      , solver_(new btSequentialImpulseConstraintSolver()) {
    world_ = engine::make_unique<btDiscreteDynamicsWorld>(
        dispatcher_.get(), broadphase_.get(),
        solver_.get(), collision_config_.get());
    glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    world_->setGravity(btVector3(0, -9.81, 0));

    // Add a plane
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0, 0, 0));
    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btMotionState* motion = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
    btRigidBody* rbody = new btRigidBody(info);
    rbodies_.push_back(RigidBodyPtr(rbody));
    world_->addRigidBody(rbody);
    auto plane_mesh = addGameObject<CubeMesh>(glm::vec3(0.5, 0.5, 0.5));
    plane_mesh->transform.set_local_scale(glm::vec3(100, 1, 100));

    // Add a camera
    addCamera<engine::FreeFlyCamera>(window(), M_PI/3, 1, 500,
                                     glm::vec3(10, 5, 0), glm::vec3(), 25, 2);
  }

  virtual void update() override {
    world_->stepSimulation(game_time().dt);
    for (auto& rbody : rbodies_) {
      btTransform t;
      rbody->getMotionState()->getWorldTransform(t);
      // apply that shit to the other shit
    }
    Scene::update();
  }

  virtual void keyAction(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
      addSphere();
    }
  }
};

#endif

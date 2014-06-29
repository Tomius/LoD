// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SCENES_BULLET_BASICS_SCENE_H_
#define LOD_SCENES_BULLET_BASICS_SCENE_H_

#include <vector>
#include <bullet/btBulletDynamicsCommon.h>
#include "../engine/scene.h"
#include "../engine/camera.h"
#include "../engine/shapes/cube_mesh.h"

using engine::shapes::CubeMesh;

class BulletBasicsScene : public engine::Scene {
  std::unique_ptr<btCollisionConfiguration> collision_config_;
  std::unique_ptr<btDispatcher> dispatcher_;
  // should Bullet examine every object, or just what close to each other
  std::unique_ptr<btBroadphaseInterface> broadphase_;
  // solve collisions, apply forces, impulses
  std::unique_ptr<btConstraintSolver> solver_;
  std::unique_ptr<btDynamicsWorld> world_;

  std::vector<std::unique_ptr<btRigidBody>> bodies_;

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
    bodies_.push_back(std::unique_ptr<btRigidBody>(rbody));
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
      , solver_(new btSequentialImpulseConstraintSolver())
      , world_(new btDiscreteDynamicsWorld(dispatcher_.get(), broadphase_.get(),
                                           solver_.get(), collision_config_.get())) {
    GLFWwindow* window = engine::GameEngine::window();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    world_->setGravity(btVector3(0, -9.81, 0));

    // Add a plane
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0, 0, 0));
    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btMotionState* motion = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
    btRigidBody* rbody = new btRigidBody(info);
    bodies_.push_back(std::unique_ptr<btRigidBody>(rbody));
    world_->addRigidBody(rbody);
    auto plane_mesh = addGameObject<CubeMesh>(glm::vec3(0.5, 0.5, 0.5));
    plane_mesh->transform.set_local_scale(glm::vec3(100, 1, 100));

    // Add a camera
    addCamera<engine::FreeFlyCamera>(window, M_PI/3, 1, 500,
                                     glm::vec3(10, 5, 0), glm::vec3(), 25, 2);
  }

  virtual void update() override {
    world_->stepSimulation(game_time().dt);
    Scene::update();
  }

  virtual void keyAction(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
      addSphere();
    }
  }
};

#endif

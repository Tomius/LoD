// Copyright (c) 2014, Tamas Csala

#ifndef LOD_BULLET_SCENE_H_
#define LOD_BULLET_SCENE_H_

#include <bullet/btBulletDynamicsCommon.h>
#include "engine/scene.h"

class BulletScene : public engine::Scene {
  std::unique_ptr<btDynamicsWorld> world;
  std::unique_ptr<btDispatcher> dispatcher;
  std::unique_ptr<btCollisionConfiguration> collision_config;
  // should Bullet examine every object, or just what close to each other
  std::unique_ptr<btBroadphaseInterface> broadphase;
  // solve collisions, apply forces, impulses
  std::unique_ptr<btConstraintSolver> solver;

  std::vector<std::unique_ptr<btRigidBody>> bodies;

 public:
  BulletScene() {
    collision_config = new btCollisionConfiguration();
    dispatcher = new btCollisionDispatcher();
    // Dynamic bounding volume tree broadphase
    // Alternatively I could use btAxisSweep3 for finite bound worlds.
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase,
                                        solver, collision_config);
    world->setGravity(btVector3(0, -9.81, 0));

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(0, 0, 0);
    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btMotionState* motion = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
    btRigidBody* rbody = new btRigidBody(info);
    bodies.push_back(rbody);
    world->addRigidBody(rbody);
  }

  virtual void update() override {
    world->stepSimulation(game_time_.dt);
  }

};

#endif

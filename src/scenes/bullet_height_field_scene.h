// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SCENES_BULLET_HEIGHT_FIELD_SCENE_H_
#define LOD_SCENES_BULLET_HEIGHT_FIELD_SCENE_H_

#include <vector>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include "../engine/misc.h"
#include "../engine/scene.h"
#include "../engine/camera.h"
#include "../engine/behaviour.h"
#include "../engine/shapes/cube_mesh.h"
#include "../engine/gui/label.h"

#include "../terrain.h"
#include "../bloom.h"
#include "./main_scene.h"

using engine::shapes::CubeMesh;

class BulletRigidBody : public engine::Behaviour {
 public:
  BulletRigidBody(GameObject* parent, const glm::vec3& pos, float mass,
                  btCollisionShape* shape, const glm::quat& rot = glm::quat())
      : Behaviour(parent), static_(mass == 0.0f) {
    shape_ = std::unique_ptr<btCollisionShape>(shape);
    btVector3 inertia(0, 0, 0);
    if (!static_) {
      shape_->calculateLocalInertia(mass, inertia);
    }
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
    motion_state_ = engine::make_unique<btDefaultMotionState>(transform);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion_state_.get(),
                                                  shape_.get(), inertia);
    rigid_body_ = engine::make_unique<btRigidBody>(info);
    scene_->world()->addRigidBody(rigid_body_.get());
  }

  virtual ~BulletRigidBody() {
    scene_->world()->removeCollisionObject(rigid_body_.get());
  }

  btRigidBody* rigid_body() { return rigid_body_.get(); }
  const btRigidBody* rigid_body() const { return rigid_body_.get(); }

 private:
  bool static_;
  std::unique_ptr<btCollisionShape> shape_;
  std::unique_ptr<btMotionState> motion_state_;
  std::unique_ptr<btRigidBody> rigid_body_;

  virtual void update() override {
    if (static_) { return; }

    btTransform t;
    t.setIdentity();
    motion_state_->getWorldTransform(t);
    const btVector3& o = t.getOrigin();
    parent_->transform()->set_pos(glm::vec3(o.x(), o.y(), o.z()));
    const btQuaternion& r = t.getRotation();
    parent_->transform()->set_rot(glm::quat(r.getW(), r.getX(),
                                         r.getY(), r.getZ()));
  }
};

class StaticPlane : public engine::GameObject {
 public:
  explicit StaticPlane(GameObject* parent) : GameObject(parent) {
    btCollisionShape* shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    addComponent<BulletRigidBody>(glm::vec3(), 0.0f, shape);
    auto plane_mesh = addComponent<CubeMesh>(glm::vec3(0.5, 0.5, 0.5));
    plane_mesh->transform()->set_local_pos(glm::vec3(0, -0.5f, 0));
    plane_mesh->transform()->set_local_scale(glm::vec3(400, 1, 400));
  }
};

class HeightField : public engine::GameObject {
 public:
  explicit HeightField(GameObject* parent)
      : GameObject(parent) {
    Terrain* terrain = addComponent<Terrain>();
    const auto& height_map = terrain->height_map();
    btCollisionShape* shape = new btHeightfieldTerrainShape(
        height_map.w(), height_map.h(), height_map.data(),
        255, 1, PHY_SHORT, false);
    addComponent<BulletRigidBody>(glm::vec3(), 0.0f, shape);
  }
};

class RedCube : public engine::GameObject {
 public:
  explicit RedCube(GameObject* parent, const glm::vec3& pos,
                   const glm::vec3& v, const glm::quat& rot)
      : GameObject(parent) {
    btVector3 half_extents(0.5f, 0.5f, 0.5f);
    btCollisionShape* shape = new btBoxShape(half_extents);
    auto rbody = addComponent<BulletRigidBody>(pos, 1.0f, shape, rot);
    rbody->rigid_body()->setLinearVelocity(btVector3(v.x, v.y, v.z));
    addComponent<CubeMesh>(glm::vec3(1.0, 0.0, 0.0));
  }
};

class BulletHeightFieldScene : public engine::Scene {
  void addSmallRedCube() {
    auto cam = camera();
    glm::vec3 pos = cam->transform()->pos() + 3.0f*cam->transform()->forward();
    addComponent<RedCube>(pos, 20.0f*cam->transform()->forward(),
                          cam->transform()->rot());
  }

 public:
  BulletHeightFieldScene() {
    glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    collision_config_ = engine::make_unique<btDefaultCollisionConfiguration>();
    dispatcher_ =
        engine::make_unique<btCollisionDispatcher>(collision_config_.get());
    // Dynamic bounding volume tree broadphase
    // Alternatively I could use btAxisSweep3 for finite bound worlds.
    broadphase_ = engine::make_unique<btDbvtBroadphase>();
    solver_ = engine::make_unique<btSequentialImpulseConstraintSolver>();
    world_ = engine::make_unique<btDiscreteDynamicsWorld>(
        dispatcher_.get(), broadphase_.get(),
        solver_.get(), collision_config_.get());
    world_->setGravity(btVector3(0, -9.81, 0));

    auto skybox = addComponent<Skybox>();
    addComponent<Terrain>();
    // addComponent<HeightField>();
    auto bloom = addComponent<BloomEffect>(skybox);
    bloom->set_group(1);

    auto cam = addComponent<engine::FreeFlyCamera>(M_PI/3, 1, 3000,
        glm::vec3(2058, 255, 2048), glm::vec3(2048, 250, 2048), 20, 10);
    set_camera(cam);

    auto label = addComponent<engine::gui::Label>(
        L"Press space to shoot a cube.", glm::vec2(0, -0.9));
    label->set_vertical_alignment(engine::gui::Font::VerticalAlignment::kCenter);
    label->set_font_size(20);

    auto label2 = addComponent<engine::gui::Label>(
        L"You can load the main scene by pressing the home button.", glm::vec2(0, -0.95));
    label2->set_vertical_alignment(engine::gui::Font::VerticalAlignment::kCenter);
    label2->set_font_size(14);
  }

  virtual void update() override {
    world_->stepSimulation(game_time().dt, 5);
    Scene::update();
  }

  virtual void keyAction(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_SPACE) {
        addSmallRedCube();
      } else if (key == GLFW_KEY_HOME) {
        engine::GameEngine::LoadScene<MainScene>();
      }
    }
  }
};

#endif

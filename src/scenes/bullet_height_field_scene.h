// Copyright (c) 2014, Tamas Csala

#ifndef LOD_SCENES_BULLET_HEIGHT_FIELD_SCENE_H_
#define LOD_SCENES_BULLET_HEIGHT_FIELD_SCENE_H_

#include <vector>
#include <algorithm>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

// fck windows.h
#undef min
#undef max

#include "../engine/misc.h"
#include "../engine/scene.h"
#include "../engine/camera.h"
#include "../engine/behaviour.h"
#include "../engine/shapes/cube_mesh.h"
#include "../engine/gui/label.h"

#include "../terrain.h"
#include "../after_effects.h"
#include "../fps_display.h"
#include "../loading_screen.h"
#include "./main_scene.h"

using engine::shapes::CubeMesh;

class BulletRigidBody : public engine::Behaviour {
 public:
  BulletRigidBody(GameObject* parent, const glm::vec3& pos, float mass,
                  btCollisionShape* shape, const glm::quat& rot = glm::quat(),
                  bool no_rotation = false)
      : Behaviour(parent), static_(mass == 0.0f), no_rot_(no_rotation) {
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
  bool static_, no_rot_;
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
    if (!no_rot_) {
      const btQuaternion& r = t.getRotation();
      parent_->transform()->set_rot(glm::quat(r.getW(), r.getX(),
                                              r.getY(), r.getZ()));
    }
  }
};

class HeightField : public engine::GameObject {
 public:
  explicit HeightField(GameObject* parent) : GameObject(parent) {
    Terrain* terrain = addComponent<Terrain>();
    const auto& height_map = terrain->height_map();
    int w = height_map.w(), h = height_map.h();
    GLubyte *data = new GLubyte[w*h];
    for (int x = 0; x < w; ++x) {
      for (int y = 0; y < h; ++y) {
        data[y*w + x] = height_map.heightAt(x, y);
      }
    }
    btCollisionShape* shape = new btHeightfieldTerrainShape{
        height_map.w(), height_map.h(), data,
        1, 0, 256, 1, PHY_UCHAR, false};
    addComponent<BulletRigidBody>(glm::vec3{height_map.w()/2.0f, 128,
                                            height_map.h()/2.0f}, 0.0f, shape);
  }
};

class RedCube : public engine::Behaviour {
 public:
  explicit RedCube(GameObject* parent, const glm::vec3& pos,
                   const glm::vec3& v, const glm::quat& rot = glm::quat{})
      : Behaviour(parent) {
    btVector3 half_extents(0.5f, 0.5f, 0.5f);
    btCollisionShape* shape = new btBoxShape(half_extents);
    auto rbody = addComponent<BulletRigidBody>(pos, 1.0f, shape, rot);
    auto rigid_body = rbody->rigid_body();
    rigid_body->setLinearVelocity(btVector3(v.x, v.y, v.z));
    rigid_body->setUserPointer(this);
    cube_mesh_ = addComponent<CubeMesh>(glm::vec3(0.5, 0.0, 0.0));
  }

  virtual void collision(const GameObject* other) override {
    const RedCube* red = dynamic_cast<const RedCube*>(other);
    if (red) {
      addColor(glm::vec3{0.0f, 0.02f, 0.02f});
    } else {
      addColor(glm::vec3{0.0f, 0.02f, 0.0f});
    }
  }

 private:
  CubeMesh* cube_mesh_;

  virtual void update() override {
    glm::vec3 color = cube_mesh_->color();
    color = glm::vec3(color.r, std::min(0.98f*color.g, 0.9f),
                               std::min(0.995f*color.b, 0.9f)));
    cube_mesh_->set_color(color);
  }

  void addColor(const glm::vec3& color) {
    cube_mesh_->set_color(glm::clamp(cube_mesh_->color() + color,
                                     glm::vec3{}, glm::vec3{1}));
  }
};

class BulletHeightFieldScene : public engine::Scene {
  void shootCube() {
    auto cam = camera();
    glm::vec3 pos = cam->transform()->pos() + 3.0f*cam->transform()->forward();
    addComponent<RedCube>(pos, 20.0f*cam->transform()->forward(),
                          cam->transform()->rot());
  }

  void dropCubes() {
    auto cam = camera();
    glm::vec3 base_pos = cam->transform()->pos() - 2.0f*cam->transform()->up();
    for(int x = -2; x <= 2; ++x) {
      for(int y = -2; y <= 2; ++y) {
        addComponent<RedCube>(base_pos + 1.5f*glm::vec3(x, 0, y), glm::vec3());
      }
    }
  }

 public:
  BulletHeightFieldScene() {
    glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    LoadingScreen().render();
    glfwSwapBuffers(window());

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
    addComponent<HeightField>();
    auto after_effects = addComponent<AfterEffects>(skybox);
    after_effects->set_group(1);

    auto cam = addComponent<engine::FreeFlyCamera>(M_PI/3, 1, 3000,
        glm::vec3(2050, 200, 2050), glm::vec3(2048, 200, 2048), 20, 5);
    set_camera(cam);

    auto label = addComponent<engine::gui::Label>(
        L"Press left click to shoot a cube.", glm::vec2(0, -0.85));
    label->set_vertical_alignment(engine::gui::Font::VerticalAlignment::kCenter);
    label->set_font_size(20);
    label->set_group(2);

    auto label2 = addComponent<engine::gui::Label>(
        L"Press space to drop a bunch of cubes.", glm::vec2(0, -0.9));
    label2->set_vertical_alignment(engine::gui::Font::VerticalAlignment::kCenter);
    label2->set_font_size(20);
    label2->set_group(2);


    auto label3 = addComponent<engine::gui::Label>(L"You can load the main "
      L"scene by pressing the home button.", glm::vec2(0, -0.95));
    label3->set_vertical_alignment(engine::gui::Font::VerticalAlignment::kCenter);
    label3->set_font_size(14);
    label3->set_group(2);

    auto fps = addComponent<FpsDisplay>();
    fps->set_group(2);
  }

  void findCollisions() {
    int num_manifolds = world_->getDispatcher()->getNumManifolds();
    for (int i = 0; i < num_manifolds; ++i) {
      btPersistentManifold* contact_manifold =
         world_->getDispatcher()->getManifoldByIndexInternal(i);
      const btCollisionObject* obA = contact_manifold->getBody0();
      const btCollisionObject* obB = contact_manifold->getBody1();

      int num_contacts = contact_manifold->getNumContacts();
      for (int j = 0; j < num_contacts; ++j) {
        btManifoldPoint& pt = contact_manifold->getContactPoint(j);
        if (pt.getDistance() < 0.0f) {
          // const btVector3& ptA = pt.getPositionWorldOnA();
          // const btVector3& ptB = pt.getPositionWorldOnB();
          // const btVector3& normalOnB = pt.m_normalWorldOnB;
          auto go1 = (engine::GameObject*)obA->getUserPointer();
          auto go2 = (engine::GameObject*)obB->getUserPointer();
          if (go1) { go1->collisionAll(go2); }
          if (go2) { go2->collisionAll(go1); }
        }
      }
    }
  }


  virtual void update() override {
    world_->stepSimulation(game_time().dt, 5);
    findCollisions();
    Scene::update();
  }

  virtual void keyAction(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_SPACE) {
        dropCubes();
      } else if (key == GLFW_KEY_HOME) {
        engine::GameEngine::LoadScene<MainScene>();
      }
    }
  }

  virtual void mouseButtonPressed(int button, int action, int mods) override {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      shootCube();
    }
  }
};

#endif

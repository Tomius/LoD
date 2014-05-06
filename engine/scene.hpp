#ifndef ENGINE_SCENE_HPP_
#define ENGINE_SCENE_HPP_

#include <vector>
#include <memory>

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"

#include "shadow.hpp"
#include "skybox.hpp"

#include "time.hpp"
#include "tpcamera.hpp"

class GameObject;

namespace engine {

class Scene {
  std::vector<std::unique_ptr<GameObject>> gameobjects_, after_effects_;

public:
  TPCamera* camera_;
  std::unique_ptr<Skybox> skybox_;
  std::unique_ptr<Shadow> shadow_;
  Timer game_time_, environment_time_;

  Scene() { }

  template<typename T>
  T* addGameObject(T* gameobj) {
    auto go = dynamic_cast<GameObject*>(gameobj);
    assert(go);
    gameobjects_.push_back(std::unique_ptr<GameObject>(go));
    return gameobj;
  }

  template<typename T>
  T* addAfterEffect(T* after_effect) {
    auto go = dynamic_cast<GameObject*>(after_effect);
    assert(go);
    after_effects_.push_back(std::unique_ptr<GameObject>(go));
    return after_effect;
  }

  Shadow* addShadow(Shadow* shadow) {
    shadow_ = std::unique_ptr<Shadow>(shadow);
    return shadow;
  }

  void addCamera(TPCamera* cam) {
    camera_ = cam;
  }

  Skybox* addSkybox(Skybox* skybox) {
    skybox_ = std::unique_ptr<Skybox>(skybox);
    return skybox;
  }

  void screenResized(const glm::mat4& projMat, GLuint w, GLuint h) {
    if(shadow_) {
      shadow_->screenResized(w, h);
    }

    if(skybox_) {
      skybox_->screenResized(projMat, w, h);
    }

    for(auto& i : gameobjects_) {
      i->screenResized(projMat, w, h);
    }
    for(auto& i : after_effects_) {
      i->screenResized(projMat, w, h);
    }
  }

private:
  void update() {
    game_time_.tick();
    environment_time_.tick();

    if(skybox_) {
      skybox_->update(environment_time_.current);
    }

    for(auto& i : gameobjects_) {
      i->update(game_time_.current);
    }

    camera_->update(game_time_.current);
  }

  void shadowRender() {
    if(!camera_)
      return;

    if(shadow_) {
      shadow_->begin(); {
        for(auto& i : gameobjects_) {
          i->shadowRender(game_time_.current, *camera_);
        }
      } shadow_->end();
    }
  }

  void render() {
    if(!camera_)
      return;

    if(skybox_) {
      skybox_->render(environment_time_.current, *camera_);
    }

    for(auto& i : gameobjects_) {
      i->render(game_time_.current, *camera_);
    }
    for(auto& i : after_effects_) {
      i->render(game_time_.current, *camera_);
    }
  }

public:
  void turn() {
    update();
    shadowRender();
    render();
  }
};

}


#endif

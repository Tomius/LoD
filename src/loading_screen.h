// Copyright (c) 2014, Tamas Csala

#ifndef LOD_LOADING_SCREEN_H_
#define LOD_LOADING_SCREEN_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/full_screen_rect.h"
#include "oglwrap/textures/texture_2D.h"
#include "oglwrap/smart_enums.h"

#include "engine/game_object.h"

class LoadingScreen {
  gl::Texture2D tex_;
  gl::FullScreenRectangle rect_;

  gl::Program prog_;

public:
  LoadingScreen() {
    gl::VertexShader vs("loading.vert");
    gl::FragmentShader fs("loading.frag");
    prog_ << vs << fs;
    prog_.link().use();

    tex_.active(0);
    tex_.bind();
    tex_.loadTexture("textures/loading.png");
    tex_.minFilter(gl::kLinear);
    tex_.magFilter(gl::kLinear);
    tex_.unbind();

    gl::UniformSampler(prog_, "uTex").set(0);

    prog_.validate();

    rect_.setupPositions(prog_ | "aPosition");
    rect_.setupTexCoords(prog_ | "aTexCoord", true);
  }

  void render() {
    prog_.use();
    tex_.active(0);
    tex_.bind();

    auto capabilies = gl::TemporarySet({{gl::kCullFace, false},
                                        {gl::kDepthTest, false}});

    rect_.render();
    tex_.unbind();
  }
};

#endif // LOD_LOADING_SCREEN_H_

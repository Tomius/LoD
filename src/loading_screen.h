// Copyright (c) 2014, Tamas Csala

#ifndef LOD_LOADING_SCREEN_H_
#define LOD_LOADING_SCREEN_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/full_screen_rect.h"
#include "oglwrap/textures/texture_2D.h"

#include "engine/gameobject.h"

class LoadingScreen {
  oglwrap::Texture2D tex_;
  oglwrap::FullScreenRectangle rect_;

  oglwrap::Program prog_;

public:
  LoadingScreen() {
    oglwrap::VertexShader vs("loading.vert");
    oglwrap::FragmentShader fs("loading.frag");
    prog_ << vs << fs;
    prog_.link().use();

    tex_.active(0);
    tex_.bind();
    tex_.loadTexture("textures/loading.png");
    tex_.minFilter(oglwrap::MinFilter::Linear);
    tex_.magFilter(oglwrap::MagFilter::Linear);
    tex_.unbind();

    oglwrap::UniformSampler(prog_, "uTex").set(0);

    prog_.validate();

    rect_.setupPositions(prog_ | "aPosition");
    rect_.setupTexCoords(prog_ | "aTexcoord", true);
  }

  void render() {
    using oglwrap::Capability;

    prog_.use();
    tex_.active(0);
    tex_.bind();

    auto capabilies = oglwrap::Context::TemporarySet({
      {Capability::CullFace, false},
      {Capability::DepthTest, false}
    });

    rect_.render();
    tex_.unbind();
  }
};

#endif // LOD_LOADING_SCREEN_H_

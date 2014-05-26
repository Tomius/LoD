// Copyright (c) 2014, Tamas Csala

#ifndef LOD_LOADING_SCREEN_H_
#define LOD_LOADING_SCREEN_H_

#include "./lod_oglwrap_config.h"
#include "oglwrap/shader.h"
#include "oglwrap/uniform.h"
#include "oglwrap/shapes/full_screen_rect.h"
#include "oglwrap/textures/texture_2D.h"
#include "oglwrap/smart_enums.h"

#include "engine/gameobject.h"

class LoadingScreen {
  oglwrap::Texture2D tex_;
  oglwrap::FullScreenRectangle rect_;

  oglwrap::Program prog_;

public:
  LoadingScreen() {
    namespace gl = oglwrap;
    using glEnum = oglwrap::SmartEnums;

    gl::VertexShader vs("loading.vert");
    gl::FragmentShader fs("loading.frag");
    prog_ << vs << fs;
    prog_.link().use();

    tex_.active(0);
    tex_.bind();
    tex_.loadTexture("textures/loading.png");
    tex_.minFilter(glEnum::Linear);
    tex_.magFilter(glEnum::Linear);
    tex_.unbind();

    gl::UniformSampler(prog_, "uTex").set(0);

    prog_.validate();

    rect_.setupPositions(prog_ | "aPosition");
    rect_.setupTexCoords(prog_ | "aTexcoord", true);
  }

  void render() {
    namespace gl = oglwrap;
    using glEnum = oglwrap::SmartEnums;

    prog_.use();
    tex_.active(0);
    tex_.bind();

    auto capabilies = gl::TemporarySet({{glEnum::CullFace, false},
                                        {glEnum::DepthTest, false}});

    rect_.render();
    tex_.unbind();
  }
};

#endif // LOD_LOADING_SCREEN_H_

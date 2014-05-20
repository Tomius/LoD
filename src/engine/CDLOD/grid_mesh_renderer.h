// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_GRID_MESH_RENDERER_H_
#define ENGINE_CDLOD_GRID_MESH_RENDERER_H_

#include "grid_mesh.h"
#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/context.h"
#include "../../oglwrap/shapes/cube.h"
#include "../../oglwrap/textures/texture_2D.h"
#include "../../oglwrap/debug/insertion.h"
#include "../../terrain_data.h" // debug
#include "../../skybox.h" // debug
#include "../../shadow.h" // debug

#include "../gameobject.h"

namespace engine {

class GridMeshRenderer {
  GridMesh mesh_;
  Skybox *skybox_;
  RawTerrainData terrain_; // debug
  oglwrap::Texture2D heightMap_, grassMaps_[2], grassNormalMap_; // debug
  oglwrap::Program prog_;
  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uShadowCP_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  oglwrap::LazyUniform<glm::vec3> uCamPos_;
  oglwrap::LazyUniform<glm::vec2> uOffset_;
  oglwrap::LazyUniform<float> uScale_;
  oglwrap::LazyUniform<int> uLevel_, uNumUsedShadowMaps_;
  Shadow* shadow_;

 public:

  GridMeshRenderer(Skybox* skybox, Shadow *shadow, GLubyte dimension)
      : skybox_(skybox)
      , terrain_("terrain/mideu.rtd0") // debug
      , uProjectionMatrix_(prog_, "uProjectionMatrix")
      , uCameraMatrix_(prog_, "uCameraMatrix")
      , uShadowCP_(prog_, "uShadowCP")
      , uSunData_(prog_, "uSunData")
      , uCamPos_(prog_, "uCamPos")
      , uOffset_(prog_, "uOffset")
      , uScale_(prog_, "uScale")
      , uLevel_(prog_, "uLevel")
      , uNumUsedShadowMaps_(prog_, "uNumUsedShadowMaps")
      , shadow_(shadow) {

    oglwrap::VertexShader vs;

    #ifdef glVertexAttribDivisor
      if (glVertexAttribDivisor)
        vs.sourceFile("grid_mesh_renderer.vert");
      else
    #endif
        vs.sourceFile("grid_mesh_renderer_no_vertex_attrib_divisor.vert");

    oglwrap::FragmentShader fs{"grid_mesh_renderer.frag"};

    prog_ << vs << fs << skybox_->sky_fs;
    prog_.link().use();

    mesh_.setupPositions(prog_ | "aPosition", dimension);

    #ifdef glVertexAttribDivisor
      if (glVertexAttribDivisor) {
        mesh_.setupRenderData(prog_ | "aRenderData");
      }
    #endif

    // begin(debug)
    using oglwrap::UniformSampler;
    using oglwrap::MinFilter;
    using oglwrap::MagFilter;
    using oglwrap::WrapMode;
    using oglwrap::Context;
    using oglwrap::PixelStorageMode;
    using oglwrap::Uniform;
    using oglwrap::UniformSampler;

    Context::PixelStore(PixelStorageMode::UnpackAlignment, 1);

    UniformSampler(prog_, "uEnvMap") = 0;
    oglwrap::UniformSampler(prog_, "uHeightMap") = 1;
    oglwrap::Uniform<glm::vec2>(prog_, "uTexSize") =
        glm::vec2(terrain_.w, terrain_.h);
    heightMap_.active(1);
    heightMap_.bind();
    {
      heightMap_.upload(
        oglwrap::PixelDataInternalFormat::R8,
        terrain_.w, terrain_.h,
        oglwrap::PixelDataFormat::Red,
        oglwrap::PixelDataType::UnsignedByte,
        terrain_.heightData.data()
      );
      heightMap_.minFilter(oglwrap::MinFilter::Linear);
      heightMap_.magFilter(oglwrap::MagFilter::Linear);
    }

    oglwrap::UniformSampler(prog_, "uGrassMap0") = 2;
    oglwrap::UniformSampler(prog_, "uGrassMap1") = 3;
    for (int i = 0; i < 2; ++i) {
      grassMaps_[i].bind();
      grassMaps_[i].loadTexture(
        i == 0 ? "textures/grass.jpg" : "textures/grass_2.jpg"
      );
      grassMaps_[i].generateMipmap();
      grassMaps_[i].maxAnisotropy();
      grassMaps_[i].minFilter(MinFilter::LinearMipmapLinear);
      grassMaps_[i].magFilter(MagFilter::Linear);
      grassMaps_[i].wrapS(WrapMode::Repeat);
      grassMaps_[i].wrapT(WrapMode::Repeat);
    }

    UniformSampler(prog_, "uGrassNormalMap").set(4);
    grassNormalMap_.bind();
    {
      grassNormalMap_.loadTexture("textures/grass_normal.jpg");
      grassNormalMap_.generateMipmap();
      grassNormalMap_.minFilter(MinFilter::LinearMipmapLinear);
      grassNormalMap_.magFilter(MagFilter::Linear);
      grassNormalMap_.wrapS(WrapMode::Repeat);
      grassNormalMap_.wrapT(WrapMode::Repeat);
    }

    UniformSampler(prog_, "uShadowMap").set(5);
    Uniform<glm::ivec2>(prog_, "uShadowAtlasSize") = shadow->getAtlasDimensions();

    oglwrap::Texture2D::Unbind();
    // end(debug)

    prog_.validate();

    prog_.unuse();
  }

  void set_dimension(GLubyte dimension) {
    prog_.use();
    mesh_.setupPositions(prog_ | "aPosition", dimension);
    clearRenderList();
    prog_.unuse();
  }

  void clearRenderList() {
    mesh_.clearRenderList();
  }

  void addToRenderList(glm::vec2 offset, float scale, int level,
                       bool tl = true, bool tr = true,
                       bool bl = true, bool br = true) {
    #ifdef glVertexAttribDivisor
      if (!glVertexAttribDivisor)
    #endif
    {
      uOffset_ = offset;
      uScale_ = scale;
      uLevel_ = level;
    }

    mesh_.addToRenderList(glm::vec4(offset, scale, level), tl, tr, bl, br);
  }

  void setupRender(const Camera& cam) {
    using oglwrap::Context;
    using oglwrap::Capability;
    using oglwrap::PolyMode;
    using oglwrap::FaceOrientation;

    prog_.use();
    uCamPos_ = cam.pos();
    uCameraMatrix_ = cam.matrix();
    uProjectionMatrix_ = cam.projectionMatrix();

    // begin(debug)
    uSunData_ = skybox_->getSunData();
    for (size_t i = 0; i < shadow_->getDepth(); ++i) {
      uShadowCP_[i] = shadow_->shadowCPs()[i];
    }
    uNumUsedShadowMaps_ = shadow_->getDepth();

    skybox_->env_map.active(0);
    skybox_->env_map.bind();
    heightMap_.active(1);
    heightMap_.bind();
    grassMaps_[0].active(2);
    grassMaps_[0].bind();
    grassMaps_[1].active(3);
    grassMaps_[1].bind();
    grassNormalMap_.active(4);
    grassNormalMap_.bind();
    shadow_->shadowTex().active(5);
    shadow_->shadowTex().bind();
    // end(debug)

    Context::FrontFace(FaceOrientation::CCW);
    Context::Enable(Capability::CullFace);
    //Context::PolygonMode(PolyMode::Line);
  }

  void render() {
    using oglwrap::Context;
    using oglwrap::Capability;
    using oglwrap::PolyMode;

    mesh_.render();
    //Context::PolygonMode(PolyMode::Fill);
    Context::Disable(Capability::CullFace);
    prog_.unuse();

    // begin(debug)
    skybox_->env_map.active(0);
    skybox_->env_map.unbind();
    heightMap_.active(1);
    heightMap_.unbind();
    grassMaps_[0].active(2);
    grassMaps_[0].unbind();
    grassMaps_[1].active(3);
    grassMaps_[1].unbind();
    grassNormalMap_.active(4);
    grassNormalMap_.unbind();
    shadow_->shadowTex().active(5);
    shadow_->shadowTex().unbind();
    // end(debug)
  }

  void getMinMaxOfArea(GLshort x, GLshort y, GLushort size,
                       GLshort *min, GLshort *max) const {
    // offset the coordinates to match the texture
    x += terrain_.w / 2;
    y += terrain_.h / 2;

    const int big_number = 9999999;
    int curr_min = big_number, curr_max = -big_number;

    for(int i = x - size/2; i <= x + size/2; ++i) {
      for(int j = y - size/2; j <= y + size/2; ++j) {
        if(terrain_.isValid(i, j)) {
          int curr_height = terrain_(i, j);
          if(curr_height < curr_min) {
            curr_min = curr_height;
          }
          if(curr_height > curr_max) {
            curr_max = curr_height;
          }
        }
      }
    }

    if(curr_min == big_number) {
      curr_min = 0;
    }
    if(curr_max == -big_number) {
      curr_max = 0;
    }

    *min = curr_min;
    *max = curr_max;
  }

  static double mix(double x, double y, double a) {
    return x*(1-a) + y*a;
  }

  double getHeight(double x, double y) const {
    // offset the coordinates to match the texture
    x += terrain_.w / 2;
    y += terrain_.h / 2;

    /*
     * fx, cy -- cx, cy
     *    |        |
     *    |        |
     * fx, fy -- cx, fy
     */

    double fx = floor(x), cx = ceil(x);
    double fy = floor(y), cy = ceil(y);

    double fh = mix(terrain_.get(fx, fy), terrain_.get(cx, fy), x-fx);
    double ch = mix(terrain_.get(fx, cy), terrain_.get(cx, cy), x-fx);

    return mix(fh, ch, y-fy);
  }

  int w() const {
    return terrain_.w;
  }

  int h() const {
    return terrain_.h;
  }
};

}

#endif

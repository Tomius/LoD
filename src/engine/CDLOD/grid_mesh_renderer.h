// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_GRID_MESH_RENDERER_H_
#define ENGINE_CDLOD_GRID_MESH_RENDERER_H_

#include "grid_mesh.h"
#include "../../oglwrap/shader.h"
#include "../../oglwrap/uniform.h"
#include "../../oglwrap/context.h"
#include "../../oglwrap/textures/texture_2D.h"
#include "../../terrain_data.h" // debug
#include "../../skybox.h" // debug

#include "../gameobject.h"

namespace engine {

class GridMeshRenderer {
  GridMesh mesh_;
  Skybox *skybox_;
  RawTerrainData terrain_; // debug
  oglwrap::Texture2D heightMap_, grassMaps_[2], grassNormalMap_; // debug
  oglwrap::Program prog_;
  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  oglwrap::LazyUniform<glm::vec3> uCamPos_;
  oglwrap::LazyUniform<glm::vec2> uOffset_;
  oglwrap::LazyUniform<float> uScale_;
  oglwrap::LazyUniform<int> uLevel_;

 public:
  static constexpr GLubyte node_dim = 128;
  GLubyte dimension_;

  GridMeshRenderer(Skybox* skybox, GLubyte dimension)
      : skybox_(skybox)
      , terrain_("terrain/mideu.rtd1") // debug
      , uProjectionMatrix_(prog_, "uProjectionMatrix")
      , uCameraMatrix_(prog_, "uCameraMatrix")
      , uSunData_(prog_, "uSunData")
      , uCamPos_(prog_, "uCamPos")
      , uOffset_(prog_, "uOffset")
      , uScale_(prog_, "uScale")
      , uLevel_(prog_, "uLevel")
      , dimension_(dimension) {

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

    oglwrap::UniformSampler(prog_, "uGrassNormalMap") = 4;
    grassNormalMap_.bind();
    {
      grassNormalMap_.loadTexture("textures/grass_normal.jpg");
      grassNormalMap_.generateMipmap();
      grassNormalMap_.minFilter(MinFilter::LinearMipmapLinear);
      grassNormalMap_.magFilter(MagFilter::Linear);
      grassNormalMap_.wrapS(WrapMode::Repeat);
      grassNormalMap_.wrapT(WrapMode::Repeat);
    }

    oglwrap::Texture2D::Unbind();
    // end(debug)

    prog_.validate();
  }

  void set_dimension(GLubyte dimension) {
    dimension_ = dimension;
    mesh_.setupPositions(prog_ | "aPosition", dimension);
  }

  void clearRenderList() {
    mesh_.clearRenderList();
  }

  void addToRenderList(glm::vec2 offset, float scale, int level,
                       bool tl = true, bool tr = true,
                       bool bl = true, bool br = true) {
    scale *= (node_dim/dimension_);
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

  void setup_render(const Camera& cam) {
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
    // end(debug)

    Context::FrontFace(FaceOrientation::CCW);
    Context::TemporaryEnable cullface(Capability::CullFace);
    //Context::PolygonMode(PolyMode::Line);
  }

  void render() {
    using oglwrap::Context;
    using oglwrap::PolyMode;

    mesh_.render();
    //Context::PolygonMode(PolyMode::Fill);
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
    // end(debug)
  }

  void getMinMaxOfArea(GLshort x, GLshort y, GLushort size,
                       GLshort *min, GLshort *max) const {
    int curr_min = 9999999, curr_max = -9999999;

    for(int i = x - size/2; i <= x + size/2; ++i) {
      for(int j = y - size/2; j <= y + size/2; ++j) {
        int curr_height = terrain_.get(i, j);
        if(curr_height < curr_min) {
          curr_min = curr_height;
        }
        if(curr_height > curr_max) {
          curr_max = curr_height;
        }
      }
    }

    *min = curr_min;
    *max = curr_max;
  }
};

}

#endif

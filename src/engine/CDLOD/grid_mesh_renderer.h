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

#include "../gameobject.h"

namespace engine {

class GridMeshRenderer {
  GridMesh mesh_;
  Skybox *skybox_;
  oglwrap::Cube debug_cube_;
  RawTerrainData terrain_; // debug
  oglwrap::Texture2D heightMap_, grassMaps_[2]; // debug
  oglwrap::Program prog_, cube_prog_;
  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_;
  oglwrap::LazyUniform<glm::mat4> cube_uProjectionMatrix_, cube_uCameraMatrix_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;
  oglwrap::LazyUniform<glm::vec3> uCamPos_;
  oglwrap::LazyUniform<glm::vec2> uOffset_, cube_uOffset_, cube_uMinMax_;
  oglwrap::LazyUniform<float> uScale_, cube_uScale_;
  oglwrap::LazyUniform<int> uLevel_, cube_uLevel_;

 public:
  static constexpr GLubyte node_dim = 128;
  GLubyte dimension_;

  GridMeshRenderer(Skybox* skybox, GLubyte dimension)
      : skybox_(skybox)
      , debug_cube_(node_dim, node_dim, node_dim)
      , terrain_("terrain/mideu.rtd0") // debug
      , uProjectionMatrix_(prog_, "uProjectionMatrix")
      , uCameraMatrix_(prog_, "uCameraMatrix")
      , cube_uProjectionMatrix_(cube_prog_, "uProjectionMatrix")
      , cube_uCameraMatrix_(cube_prog_, "uCameraMatrix")
      , uSunData_(prog_, "uSunData")
      , uCamPos_(prog_, "uCamPos")
      , uOffset_(prog_, "uOffset")
      , cube_uOffset_(cube_prog_, "uOffset")
      , cube_uMinMax_(cube_prog_, "uMinMax")
      , uScale_(prog_, "uScale")
      , cube_uScale_(cube_prog_, "uScale")
      , uLevel_(prog_, "uLevel")
      , cube_uLevel_(cube_prog_, "uLevel")
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

    oglwrap::Texture2D::Unbind();
    // end(debug)

    prog_.validate();

    prog_.unuse();

    oglwrap::VertexShader cube_vs{"debug_cube.vert"};
    oglwrap::FragmentShader cube_fs{"debug_cube.frag"};

    cube_prog_ << cube_vs << cube_fs;
    cube_prog_.link().use();

    debug_cube_.setupPositions(cube_prog_ | "aPosition");
  }

  void set_dimension(GLubyte dimension) {
    dimension_ = dimension;
    mesh_.setupPositions(prog_ | "aPosition", dimension);
  }

  void clearRenderList() {
    mesh_.clearRenderList();
  }

  void addToRenderList(const Camera& cam, glm::vec2 offset,
                       float scale, int level, glm::vec2 min_max,
                       bool tl = true, bool tr = true,
                       bool bl = true, bool br = true) {
    using oglwrap::Context;
    using oglwrap::Capability;
    using oglwrap::PolyMode;
    using oglwrap::FaceOrientation;

    float size = scale * node_dim;
    scale *= (node_dim/dimension_);

    #ifdef glVertexAttribDivisor
      if (!glVertexAttribDivisor)
    #endif
    {
      prog_.use();
      uCamPos_ = cam.pos();
      uCameraMatrix_ = cam.matrix();
      uProjectionMatrix_ = cam.projectionMatrix();

      uOffset_ = offset;
      uScale_ = scale;
      uLevel_ = level;

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
      // end(debug)

      Context::FrontFace(FaceOrientation::CCW);
      Context::Enable(Capability::CullFace);
      //Context::PolygonMode(PolyMode::Line);
    }

    mesh_.addToRenderList(glm::vec4(offset, scale, level), tl, tr, bl, br);

    #ifdef glVertexAttribDivisor
      if (!glVertexAttribDivisor)
    #endif
    {
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
      // end(debug)
    }

    // Render debug aabb-s
    cube_prog_.use();
    cube_uCameraMatrix_ = cam.matrix();
    cube_uProjectionMatrix_ = cam.projectionMatrix();
    cube_uScale_ = scale/2;
    cube_uLevel_ = level;
    cube_uMinMax_ = min_max;

    Context::Enable(Capability::CullFace);
    Context::FrontFace(debug_cube_.faceWinding());
    Context::PolygonMode(PolyMode::Line);

    if(tl) {
      cube_uOffset_ = offset + glm::vec2(-size/4, size/4);
      debug_cube_.render();
    }
    if(tr) {
      cube_uOffset_ = offset + glm::vec2(size/4, size/4);
      debug_cube_.render();
    }
    if(bl) {
      cube_uOffset_ = offset + glm::vec2(-size/4, -size/4);
      debug_cube_.render();
    }
    if(br) {
      cube_uOffset_ = offset + glm::vec2(size/4, -size/4);
      debug_cube_.render();
    }

    Context::PolygonMode(PolyMode::Fill);
    Context::Disable(Capability::CullFace);

    cube_prog_.unuse();
  }

  void render(const Camera& cam) {
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
    // end(debug)

    Context::FrontFace(FaceOrientation::CCW);
    Context::Enable(Capability::CullFace);
    //Context::PolygonMode(PolyMode::Line);

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
};

}

#endif

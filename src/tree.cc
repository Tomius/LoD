// Copyright (c) 2014, Tamas Csala

#include "./tree.h"
#include "engine/scene.h"
#include "oglwrap/debug/insertion.h"

Tree::Tree(GameObject *parent, const engine::HeightMapInterface& height_map)
    : GameObject(parent)
    , mesh_{{"models/trees/massive_swamptree_01_a.obj",
            aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs |
            aiProcess_PreTransformVertices},
            {"models/trees/massive_swamptree_01_b.obj",
            aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs |
            aiProcess_PreTransformVertices},
            {"models/trees/cedar_01_a_source.obj",
            aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs |
            aiProcess_PreTransformVertices}}
    , prog_(scene_->shader_manager()->get("tree.vert"),
            scene_->shader_manager()->get("tree.frag"))
    , shadow_prog_(scene_->shader_manager()->get("tree_shadow.vert"),
                   scene_->shader_manager()->get("tree_shadow.frag"))
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uModelCameraMatrix_(prog_, "uModelCameraMatrix")
    , uNormalMatrix_(prog_, "uNormalMatrix")
    , shadow_uMCP_(shadow_prog_, "uMCP") {
  shadow_prog_.use();
  gl::UniformSampler(shadow_prog_, "uDiffuseTexture").set(1);
  shadow_prog_.validate();

  prog_.use();

  for (int i = 0; i < kTreeTypeNum; ++i) {
    mesh_[i].setupPositions(prog_ | "aPosition");
    mesh_[i].setupTexCoords(prog_ | "aTexCoord");
    mesh_[i].setupNormals(prog_ | "aNormal");
    mesh_[i].setupDiffuseTextures(1);
  }

  gl::UniformSampler(prog_, "uDiffuseTexture").set(1);

  prog_.validate();

  // Get the trees' positions.
  const int kTreeDist = 150;
  glm::vec2 extent = height_map.extent();
  for (int i = kTreeDist; i + kTreeDist < extent.x; i += kTreeDist) {
    for (int j = kTreeDist; j + kTreeDist < extent.y; j += kTreeDist) {
      glm::ivec2 coord = glm::ivec2(i + rand()%(kTreeDist/2) - kTreeDist/4,
                                    j + rand()%(kTreeDist/2) - kTreeDist/4);
      glm::vec3 pos =
        glm::vec3(coord.x, height_map.heightAt(coord.x, coord.y)-1, coord.y);
      glm::vec3 scale = glm::vec3(1.0f + rand() / RAND_MAX,
                                  1.0f + rand() / RAND_MAX,
                                  1.0f + rand() / RAND_MAX) * 2.0f;

      float rotation = 2*M_PI * rand() / RAND_MAX;

      glm::mat4 matrix = glm::rotate(glm::mat4(), rotation, glm::vec3(0, 1, 0));
      matrix[3] = glm::vec4(pos, 1);
      matrix = glm::scale(matrix, scale);

      int type = rand() % kTreeTypeNum;

      engine::BoundingBox bbox = mesh_[type].boundingBox(matrix);
      glm::vec4 bsphere = mesh_[type].bSphere();
      bsphere.w *= 1.2;  // removes peter panning (but decreases quality)

      trees_.push_back(TreeInfo{type, matrix, bsphere, bbox});
    }
  }
}

void Tree::shadowRender() {
  shadow_prog_.use();

  auto shadow = scene_->shadow();
  auto cullface = gl::TemporaryDisable(gl::kCullFace);

  const auto& cam = *scene_->camera();
  auto campos = cam.pos();
  for (size_t i = 0; i < trees_.size() &&
      shadow->getDepth() < shadow->getMaxDepth(); i++) {
    if (glm::length(glm::vec3(trees_[i].mat[3]) - campos) < 150) {
      shadow_uMCP_ = shadow->modelCamProjMat(
          trees_[i].bsphere, trees_[i].mat, glm::mat4{});
      mesh_[trees_[i].type].render();
      shadow->push();
    }
  }
}

void Tree::render() {
  prog_.use();
  prog_.update();

  const auto& cam = *scene_->camera();
  uProjectionMatrix_ = cam.projectionMatrix();

  auto blend = gl::TemporaryEnable(gl::kBlend);
  auto cullface = gl::TemporaryDisable(gl::kCullFace);
  gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

  auto campos = cam.pos();
  auto cam_mx = cam.matrix();
  auto frustum = cam.frustum();
  for (size_t i = 0; i < trees_.size(); i++) {
    // Check for visibility
    if (!trees_[i].bbox.collidesWithFrustum(frustum) ||
      glm::length(glm::vec3(trees_[i].mat[3]) - campos) > 1500) {
      continue;
    }

    auto& mesh = mesh_[trees_[i].type];
    glm::mat4 model_mx = trees_[i].mat;
    uModelCameraMatrix_.set(cam_mx * model_mx);
    uNormalMatrix_.set(glm::inverse(glm::mat3(model_mx)));
    mesh.render();
  }
}

// Copyright (c) 2014, Tamas Csala

#include "tree.h"
#include "engine/scene.h"

Tree::Tree(const engine::HeightMapInterface& height_map,
           Skybox *skybox, Shadow *shadow)
  : mesh_{{"models/trees/swamptree.dae",
          aiProcessPreset_TargetRealtime_Quality |
          aiProcess_FlipUVs},
          {"models/trees/tree.obj",
          aiProcessPreset_TargetRealtime_Quality |
          aiProcess_FlipUVs}
         }
  , vs_("tree.vert")
  , shadow_vs_("tree_shadow.vert")
  , fs_("tree.frag")
  , shadow_fs_("tree_shadow.frag")
  , uProjectionMatrix_(prog_, "uProjectionMatrix")
  , uModelCameraMatrix_(prog_, "uModelCameraMatrix")
  , uNormalMatrix_(prog_, "uNormalMatrix")
  , shadow_uMCP_(shadow_prog_, "uMCP")
  , uSunData_(prog_, "uSunData")
  , skybox_((assert(skybox), skybox))
  , shadow_((assert(shadow), shadow)) {

  shadow_prog_ << shadow_vs_ << shadow_fs_;
  shadow_prog_.link().use();

  gl::UniformSampler(shadow_prog_, "uDiffuseTexture").set(1);

  shadow_prog_.validate();

  prog_ << vs_ << fs_ << skybox_->sky_fs();
  prog_.link().use();

  for(int i = 0; i < kTreeTypeNum; ++i) {
    mesh_[i].setupPositions(prog_ | "aPosition");
    mesh_[i].setupTexCoords(prog_ | "aTexCoord");
    mesh_[i].setupNormals(prog_ | "aNormal");
    mesh_[i].setupDiffuseTextures(1);
  }

  gl::UniformSampler(prog_, "uDiffuseTexture").set(1);

  prog_.validate();

  // Get the trees' positions.
  const int kTreeDist = 256;
  for (int i = 2*kTreeDist; i + 2*kTreeDist < height_map.h(); i += kTreeDist) {
    for (int j = 2*kTreeDist; j + 2*kTreeDist < height_map.w(); j += kTreeDist) {
      glm::ivec2 coord = glm::ivec2(i + rand()%(kTreeDist/2) - kTreeDist/4,
                                    j + rand()%(kTreeDist/2) - kTreeDist/4);
      glm::vec3 pos =
        glm::vec3(coord.x, height_map.heightAt(coord.x, coord.y)-1.0f, coord.y);
      glm::vec3 scale = glm::vec3(
                          1.0f + rand() / RAND_MAX,
                          1.0f + rand() / RAND_MAX,
                          1.0f + rand() / RAND_MAX
                        ) * 2.0f;

      float rotation = 360.0f * rand() / RAND_MAX;

      glm::mat4 matrix = glm::rotate(glm::mat4(), rotation, glm::vec3(0, 1, 0));
      matrix[3] = glm::vec4(pos, 1);
      matrix = glm::scale(matrix, scale);

      int type = rand() % kTreeTypeNum;

      engine::BoundingBox bbox = mesh_[type].boundingBox(matrix);
      glm::vec4 bsphere = mesh_[type].bSphere();
      bsphere.w *= 1.5; // removes peter panning (but decreases quality)

      trees_.push_back(TreeInfo{type, matrix, bsphere, bbox});
    }
  }
}

void Tree::shadowRender(const engine::Scene& scene) {
  shadow_prog_.use();

  auto cullface = gl::TemporaryDisable(gl::kCullFace);

  const auto& cam = *scene.camera();
  auto campos = cam.pos();
  for (size_t i = 0; i < trees_.size() &&
      shadow_->getDepth() < shadow_->getMaxDepth(); i++) {

   if (glm::length(glm::vec3(trees_[i].mat[3]) - campos) < 300) {
      shadow_uMCP_ = shadow_->modelCamProjMat(
        skybox_->getSunPos(),
        trees_[i].bsphere,
        trees_[i].mat,
        glm::mat4()
      );
      mesh_[trees_[i].type].render();
      shadow_->push();
    }
  }
}

void Tree::render(const engine::Scene& scene) {
  prog_.use();
  uSunData_.set(skybox_->getSunData());

  const auto& cam = *scene.camera();
  uProjectionMatrix_ = cam.projectionMatrix();

  auto blend = gl::TemporaryEnable(gl::kBlend);
  auto cullface = gl::TemporaryDisable(gl::kCullFace);
  gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

  auto campos = cam.pos();
  auto cam_mx = cam.matrix();
  auto frustum = cam.frustum();
  for (size_t i = 0; i < trees_.size(); i++) {
    // Check for visibility (is it behind to camera?)
    if(!trees_[i].bbox.collidesWithFrustum(frustum) ||
      glm::length(glm::vec3(trees_[i].mat[3]) - campos) > 1500) {
      continue;
    }

    glm::mat4 model_mx = trees_[i].mat;
    uModelCameraMatrix_.set(cam_mx * model_mx);
    uNormalMatrix_.set(glm::inverse(glm::mat3(model_mx)));
    mesh_[trees_[i].type].render();
  }
}

// Copyright (c) 2014, Tamas Csala

#include "./tree.h"
#include "engine/scene.h"
#include "oglwrap/debug/insertion.h"

Tree::Tree(GameObject *parent, const engine::HeightMapInterface& height_map)
    : GameObject(parent)
    , prog_(scene_->shader_manager()->get("tree.vert"),
            scene_->shader_manager()->get("tree.frag"))
    , shadow_prog_(scene_->shader_manager()->get("tree_shadow.vert"),
                   scene_->shader_manager()->get("tree_shadow.frag"))
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uModelCameraMatrix_(prog_, "uModelCameraMatrix")
    , uNormalMatrix_(prog_, "uNormalMatrix")
    , shadow_uMCP_(shadow_prog_, "uMCP") {
  gl::Use(shadow_prog_);
  gl::UniformSampler(shadow_prog_, "uDiffuseTexture").set(0);
  shadow_prog_.validate();

  gl::Use(prog_);

  meshes_[0] = engine::make_unique<engine::MeshRenderer>(
    "src/resources/models/trees/massive_swamptree_01_a.obj",
    aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs |
    aiProcess_PreTransformVertices);
  meshes_[1] = engine::make_unique<engine::MeshRenderer>(
    "src/resources/models/trees/massive_swamptree_01_b.obj",
    aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs |
    aiProcess_PreTransformVertices);
  meshes_[2] = engine::make_unique<engine::MeshRenderer>(
    "src/resources/models/trees/cedar_01_a_source.obj",
    aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs |
    aiProcess_PreTransformVertices);

  for (unsigned i = 0; i < meshes_.size(); ++i) {
    meshes_[i]->setupPositions(prog_ | "aPosition");
    meshes_[i]->setupTexCoords(prog_ | "aTexCoord");
    meshes_[i]->setupNormals(prog_ | "aNormal");
    meshes_[i]->setupDiffuseTextures(0);
  }

  gl::UniformSampler(prog_, "uDiffuseTexture").set(0);

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

      int type = rand() % meshes_.size();

      engine::BoundingBox bbox = meshes_[type]->boundingBox(matrix);
      glm::vec4 bsphere = meshes_[type]->bSphere();
      bsphere.w *= 1.2;  // removes peter panning (but decreases quality)

      trees_.push_back(TreeInfo{type, matrix, bsphere, bbox});
    }
  }
}

void Tree::shadowRender() {
  gl::Use(shadow_prog_);

  auto shadow = scene_->shadow();
  gl::TemporaryDisable cullface{gl::kCullFace};

  const auto& cam = *scene_->camera();
  auto campos = cam.transform()->pos();
  for (size_t i = 0; i < trees_.size() &&
      shadow->getDepth() < shadow->getMaxDepth(); i++) {
    if (glm::length(glm::vec3(trees_[i].mat[3]) - campos) < 150) {
      shadow_uMCP_ = shadow->modelCamProjMat(
          trees_[i].bsphere, trees_[i].mat, glm::mat4{});
      meshes_[trees_[i].type]->render();
      shadow->push();
    }
  }
}

void Tree::render() {
  gl::Use(prog_);
  prog_.update();

  const auto& cam = *scene_->camera();
  uProjectionMatrix_ = cam.projectionMatrix();

  gl::TemporarySet capabilities{{{gl::kBlend, true},
                                 {gl::kCullFace, false}}};
  gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

  auto campos = cam.transform()->pos();
  auto cam_mx = cam.cameraMatrix();
  auto frustum = cam.frustum();
  for (size_t i = 0; i < trees_.size(); i++) {
    // Check for visibility
    if (!trees_[i].bbox.collidesWithFrustum(frustum) ||
      glm::length(glm::vec3(trees_[i].mat[3]) - campos) > 1500) {
      continue;
    }

    auto& mesh = meshes_[trees_[i].type];
    glm::mat4 model_mx = trees_[i].mat;
    uModelCameraMatrix_.set(cam_mx * model_mx);
    uNormalMatrix_.set(glm::inverse(glm::mat3(model_mx)));
    mesh->render();
  }
}

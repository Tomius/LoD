#include "tree.hpp"

Tree::Tree(Skybox& skybox, const Terrain& terrain)
  : mesh_("models/trees/tree.obj",
          aiProcessPreset_TargetRealtime_MaxQuality |
          aiProcess_FlipUVs
         )
  , vs_("tree.vert")
  , shadow_vs_("tree_shadow.vert")
  , fs_("tree.frag")
  , shadow_fs_("tree_shadow.frag")
  , uProjectionMatrix_(prog_, "uProjectionMatrix")
  , uModelCameraMatrix_(prog_, "uModelCameraMatrix")
  , uNormalMatrix_(prog_, "uNormalMatrix")
  , shadow_uMCP_(shadow_prog_, "uMCP")
  , uSunData_(prog_, "uSunData")
  , skybox_(skybox) {

  shadow_prog_ << shadow_vs_ << shadow_fs_;
  shadow_prog_.link().use();

  mesh_.setupDiffuseTextures(1);
  oglwrap::UniformSampler(shadow_prog_, "uDiffuseTexture").set(1);

  prog_ << vs_ << fs_ << skybox_.sky_fs;
  prog_.link().use();

  mesh_.setupPositions(prog_ | "aPosition");
  mesh_.setupTexCoords(prog_ | "aTexCoord");
  mesh_.setupNormals(prog_ | "aNormal");
  oglwrap::UniformSampler(prog_, "uEnvMap").set(0);
  oglwrap::UniformSampler(prog_, "uDiffuseTexture").set(1);

  // Get the trees' positions.
  srand(5);
  scales_ = terrain.getScales();
  const int kTreeDist = 200;
  for(int i = -terrain.h/2; i < terrain.h/2; i += kTreeDist) {
    for(int j = -terrain.w / 2; j < terrain.w / 2; j += kTreeDist) {
      glm::ivec2 coord = glm::ivec2(i + rand()%(kTreeDist/2) - kTreeDist/4,
                                    j + rand()%(kTreeDist/2) - kTreeDist/4);
      glm::vec3 pos = scales_ * glm::vec3(coord.x, terrain.fetchHeight(coord), coord.y);
      glm::vec3 scale = glm::vec3(
                          1.0f + rand() / RAND_MAX,
                          1.0f + rand() / RAND_MAX,
                          1.0f + rand() / RAND_MAX
                        ) * 2.0f;

      float rotation = 360.0f * rand() / RAND_MAX;

      glm::mat4 matrix = glm::rotate(glm::mat4(), rotation, glm::vec3(0, 1, 0));
      matrix[3] = glm::vec4(pos, 1);
      matrix = glm::scale(matrix, scale);

      trees_.push_back(TreeInfo(pos, matrix));
    }
  }
}

void Tree::resize(glm::mat4 projMat) {
  prog_.use();
  uProjectionMatrix_ = projMat;
}

void Tree::shadowRender(float time, const oglwrap::Camera& cam, Shadow& shadow) {
  shadow_prog_.use();

  auto campos = cam.getPos();
  for(size_t i = 0; i < trees_.size() && shadow.getDepth() + 1 < shadow.getMaxDepth(); i++) {
    if(glm::length(campos - trees_[i].pos) < std::max(scales_.x, scales_.z) * (300 - PERFORMANCE * 50)) {
      shadow_uMCP_ = 
        shadow.modelCamProjMat(skybox_.getSunPos(time), mesh_.bSphere(), trees_[i].mat, mesh_.worldTransform());
      mesh_.render();
      shadow.push();
    }
  }
}

void Tree::render(float time, const oglwrap::Camera& cam) {
  prog_.use();
  uSunData_.set(skybox_.getSunData(time));
  skybox_.env_map.active(0);
  skybox_.env_map.bind();

  gl(Enable(GL_BLEND));
  gl(BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  auto campos = cam.getPos();
  auto cam_mx = cam.cameraMatrix();
  for(size_t i = 0; i < trees_.size(); i++) {
    // Check for visibility (is it behind to camera?)
    glm::vec3 diff = trees_[i].pos - campos;
    float len_diff = glm::length(diff);
    if(len_diff > 10 && glm::dot(cam.getForward(), diff) < 0)
        continue;

    // Render only if its in range.
    if(len_diff < std::max(scales_.x, scales_.z) * (800 - PERFORMANCE * 100)) {
      glm::mat4 model_mx = trees_[i].mat * mesh_.worldTransform();
      uModelCameraMatrix_.set(cam_mx * model_mx);
      uNormalMatrix_.set(glm::inverse(glm::mat3(model_mx)));
      mesh_.render();
    }
  }

  gl(Disable(GL_BLEND));

  skybox_.env_map.active(0);
  skybox_.env_map.unbind();
}

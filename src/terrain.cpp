#include "terrain.hpp"

using namespace oglwrap;

static const glm::vec3 scale_vector = glm::vec3(0.5, 0.5, 0.5);

Terrain::Terrain(Skybox& skybox)
  : vs_("terrain.vert")
  , fs_("terrain.frag")
  , uProjectionMatrix_(prog_, "uProjectionMatrix")
  , uCameraMatrix_(prog_, "uCameraMatrix")
  , uSunData_(prog_, "uSunData")
  , uScales_(prog_, "uScales")
  , uOffset_(prog_, "uOffset")
  , uMipmapLevel_(prog_, "uMipmapLevel")
  , uHeightMap_(prog_, "uHeightMap")
  , uGrassMap_(prog_, "uGrassMap")
  , uGrassNormalMap_(prog_, "uGrassNormalMap")
  , mesh_("terrain/mideu.rtd")
  , skybox_(skybox)
  , w_(mesh_.w)
  , h_(mesh_.h)
  , w(w_)
  , h(h_) {

  prog_ << vs_ << fs_ << skybox_.sky_fs;
  prog_.link().use();

  uHeightMap_.set(0);
  uGrassMap_[0].set(1);
  uGrassMap_[1].set(2);
  uGrassNormalMap_.set(3);

  uScales_ = scale_vector;
}

glm::vec3 Terrain::getScales() const {
  return scale_vector;
}

void Terrain::reshape(const glm::mat4& projMat) {
  prog_.use();
  uProjectionMatrix_ = projMat;
}

void Terrain::render(float time, const glm::mat4& cam_mat, const glm::vec3& cam_pos) {
  prog_.use();
  uCameraMatrix_.set(cam_mat);
  uSunData_.set(skybox_.getSunData(time));
  skybox_.env_map.active(0);
  skybox_.env_map.bind();

  mesh_.render(cam_pos, uOffset_, uMipmapLevel_);

  skybox_.env_map.unbind();
}

unsigned char Terrain::fetchHeight(glm::ivec2 v) const {
  return mesh_.fetchHeight(v);
}

double Terrain::getHeight(double x, double y) const {
  return mesh_.getHeight(x, y);
}

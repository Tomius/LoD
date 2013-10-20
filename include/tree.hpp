#ifndef LOD_TREE_HPP_
#define LOD_TREE_HPP_

#if defined(__APPLE__)
#include <OpenGL/glew.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>
#endif

#include <cstdio>
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/mesh/mesh.hpp"
#include "oglwrap/utils/camera.hpp"

#include "charmove.hpp"
#include "skybox.hpp"
#include "terrain.hpp"
#include "shadow.hpp"

/* 0 -> max quality
   4 -> max performance */
extern const int PERFORMANCE;

class Tree {
  oglwrap::Mesh mesh_;
  oglwrap::Program prog_, shadow_prog_;

  oglwrap::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uModelMatrix_;
  oglwrap::LazyUniform<glm::mat4> shadow_uMCP_;
  oglwrap::LazyUniform<glm::vec4> uSunData_;

  glm::vec3 scales_;
  Skybox& skybox_;

  struct TreeInfo {
    glm::vec3 pos;
    glm::mat4 mat;

    TreeInfo(const glm::vec3& pos, const glm::mat4& mat)
      : pos(pos), mat(mat) {
    }
  };

  std::vector<TreeInfo> trees_;

public:
  Tree(Skybox& skybox, const Terrain& terrain)
    : mesh_("models/trees/trees_0/tree_1.obj",
           aiProcessPreset_TargetRealtime_MaxQuality |
           aiProcess_FlipUVs
          )
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uModelMatrix_(prog_, "uModelMatrix")
    , shadow_uMCP_(shadow_prog_, "uMCP")
    , uSunData_(prog_, "uSunData")
    , skybox_(skybox) {

    shadow_prog_.attachShader(oglwrap::VertexShader("tree_shadow.vert"));
    shadow_prog_.attachShader(oglwrap::FragmentShader("tree_shadow.frag"));
    shadow_prog_.link().use();

    mesh_.setupDiffuseTextures(1);
    oglwrap::UniformSampler(shadow_prog_, "uDiffuseTexture").set(1);

    prog_.attachShader(oglwrap::VertexShader("tree.vert"));
    prog_.attachShader(oglwrap::FragmentShader("tree.frag"));
    prog_.attachShader(skybox_.sky_fs);
    prog_.link().use();

    mesh_.setupPositions(prog_ | "vPosition");
    mesh_.setupTexCoords(prog_ | "vTexCoord");
    mesh_.setupNormals(prog_ | "vNormal");
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
                          );

        float rotation = 360.0f * rand() / RAND_MAX;

        glm::mat4 matrix = glm::rotate(glm::mat4(), rotation, glm::vec3(0, 1, 0));
        matrix[3] = glm::vec4(pos, 1);
        matrix = glm::scale(matrix, scale);

        trees_.push_back(TreeInfo(pos, matrix));
      }
    }
  }

  void resize(glm::mat4 projMat) {
    prog_.use();
    uProjectionMatrix_ = projMat;
  }

  void shadowRender(float time, const oglwrap::Camera& cam, Shadow& shadow) {
    shadow_prog_.use();

    auto campos = cam.getPos();
    for(size_t i = 0; i < trees_.size() && shadow.getDepth() + 1 < shadow.getMaxDepth(); i++) {
      if(glm::length(campos - trees_[i].pos) < std::max(scales_.x, scales_.z) * (300 - PERFORMANCE * 50)) {
        const glm::mat4& modelMat = trees_[i].mat * mesh_.worldTransform();
        shadow_uMCP_ = shadow.modelCamProjMat(skybox_.getSunPos(time), mesh_.bSphere(), modelMat);
        mesh_.render();
        shadow.push();
      }
    }
  }

  void render(float time, const oglwrap::Camera& cam) {
    prog_.use();
    uCameraMatrix_.set(cam.cameraMatrix());
    uSunData_.set(skybox_.getSunData(time));
    skybox_.env_map.active(0);
    skybox_.env_map.bind();

    gl( Enable(GL_BLEND) );
    gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );

    auto campos = cam.getPos();
    for(size_t i = 0; i < trees_.size(); i++) {
      if(glm::length(campos - trees_[i].pos) < std::max(scales_.x, scales_.z) * (800 - PERFORMANCE * 100)) {
        uModelMatrix_.set(trees_[i].mat * mesh_.worldTransform());
        mesh_.render();
      }
    }

    gl( Disable(GL_BLEND) );

    skybox_.env_map.active(0);
    skybox_.env_map.unbind();
  }
};

#endif // LOD_TREE_HPP_

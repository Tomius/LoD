#ifndef LOD_HAIR_HPP_
#define LOD_HAIR_HPP__

#if defined(__APPLE__)
  #include <OpenGL/glew.h>
#else
  #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #include <windows.h>
  #endif
    #include <GL/glew.h>
#endif

#include "oglwrap/oglwrap.hpp"
#include "oglwrap/assimp.hpp"
#include "charmove.hpp"
#include "oglwrap/mesh/skinningData.hpp"


class Hair {
  struct HairSegment {
    oglwrap::ExternalBone node;
    glm::vec3 velocity, pos;
    float length;

    HairSegment* parent;
    std::vector<HairSegment> child;

    HairSegment(HairSegment* _parent, const aiNode* _node, glm::vec3 parent_pos);
  };

  HairSegment* root_;
  const oglwrap::CharacterMovement& charmove_;

  void InitNode(HairSegment* parent, int id);
  void UpdateTree(HairSegment* node, float time, float gravity);
public:
  Hair(const std::string& name, oglwrap::ExternalBone& root_ebone, const oglwrap::CharacterMovement& charmove);
  void Update(glm::vec3 pinned_pos, float time, float gravity);
};

#endif // LOD_HAIR_HPP_

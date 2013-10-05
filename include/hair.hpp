#ifndef LOD_HAIR_HPP_
#define LOD_HAIR_HPP_

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
#include "oglwrap/mesh/skinningData.hpp"
#include "charmove.hpp"

const float kHairSimulationDrag = 0.1f;

class Hair {
  struct HairSegment;

  struct BasicHairSegment {
    glm::vec3 pos;
    std::vector<HairSegment> child;
  };

  struct HairSegment : public BasicHairSegment {
    oglwrap::ExternalBone bone;
    glm::vec3 velocity, pos;
    float length;

    BasicHairSegment* parent;

    HairSegment(BasicHairSegment* _parent,
                oglwrap::ExternalBone& ebone);
  };

  struct RootHairSegment : public BasicHairSegment {
    oglwrap::ExternalBoneTree bone;

    RootHairSegment(const oglwrap::ExternalBoneTree& root_ebone)
      : bone(root_ebone)
    { }
  };

  RootHairSegment root_;
  const oglwrap::CharacterMovement& charmove_;
  glm::mat4 inverse_model_matrix_;

  void updateNode(HairSegment& node,
                  float time,
                  float gravity,
                  const glm::mat4& parent_transform);

public:

  Hair(const oglwrap::ExternalBoneTree& root_ebone,
       const oglwrap::CharacterMovement& charmove);

  void update(float time, float gravity);
};

#endif // LOD_HAIR_HPP_

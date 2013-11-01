#ifndef LOD_HAIR_HPP_
#define LOD_HAIR_HPP_

#include "oglwrap_config.hpp"
#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
#include "oglwrap/assimp.hpp"
#include "oglwrap/mesh/skinningData.hpp"
#include "charmove.hpp"

const float kHairSimulationDrag = 0.1f;

class Hair {
  struct HairSegment;

  struct BasicHairSegment {
    glm::vec3 pos, bind_pose_pos;
    std::vector<HairSegment> child;
  };

  struct HairSegment : public BasicHairSegment {
    oglwrap::ExternalBone bone;
    glm::vec3 velocity, pos;
    float length;

    BasicHairSegment* parent;

    HairSegment(BasicHairSegment* _parent,
                oglwrap::ExternalBone& ebone,
                const Hair* hair);
  };

  friend HairSegment;

  struct RootHairSegment : public BasicHairSegment {
    oglwrap::ExternalBoneTree bone;

    RootHairSegment(const oglwrap::ExternalBoneTree& root_ebone)
      : bone(root_ebone)
    { }
  };

  RootHairSegment root_;
  const CharacterMovement& charmove_;
  glm::mat4 inverse_model_matrix_;

  void updateNode(HairSegment& node,
                  float time,
                  float gravity,
                  const glm::mat4& parent_transform);

public:

  Hair(const oglwrap::ExternalBoneTree& root_ebone,
       const CharacterMovement& charmove);

  void update(float time, float gravity);
};

#endif // LOD_HAIR_HPP_

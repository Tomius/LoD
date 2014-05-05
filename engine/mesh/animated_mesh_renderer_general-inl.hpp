#ifndef ENGINE_MESH_ANIMATED_MESH_RENDERER_GENERAL_INL_HPP_
#define ENGINE_MESH_ANIMATED_MESH_RENDERER_GENERAL_INL_HPP_

#include "animated_mesh_renderer.hpp"

namespace engine {

inline AnimatedMeshRenderer::AnimatedMeshRenderer(
                                  const std::string& filename,
                                  oglwrap::Bitfield<aiPostProcessSteps> flags)
  : MeshRenderer(filename, flags)
  , skinning_data_(scene_->mNumMeshes)
  , anim_ended_callback_(nullptr) {
}

} // namespace engine

#endif // ENGINE_MESH_ANIMATED_MESH_RENDERER_GENERAL_INL_HPP_

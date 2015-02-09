// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SHADER_MANAGER_INL_H_
#define ENGINE_SHADER_MANAGER_INL_H_

#include <string>
#include "./shader_manager.h"

namespace engine {

template<typename... Args>
ShaderFile* ShaderManager::load(Args&&... args) {
  auto shader = new ShaderFile{std::forward<Args>(args)...};
  shaders_[shader->source_file()] = std::unique_ptr<ShaderFile>{shader};
  return shader;
}

inline ShaderFile* ShaderManager::get(const std::string& filename) {
  auto iter = shaders_.find(filename);
  if (iter != shaders_.end()) {
    return iter->second.get();
  } else {
    ShaderFile* shader = load(filename);
    return shader;
  }
}

inline ShaderFile* ShaderManager::publish(const std::string& filename,
                                          const gl::ShaderSource& src) {
  return load(filename, src);
}

}  // namespace engine

#endif


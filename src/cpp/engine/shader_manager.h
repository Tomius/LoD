// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SHADER_MANAGER_H_
#define ENGINE_SHADER_MANAGER_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "./oglwrap_config.h"
#include "../oglwrap/shader.h"
#include "../oglwrap/oglwrap.h"

namespace engine {

inline gl::ShaderType shader_type(const std::string& filename) {
  size_t dot_position = filename.find_last_of('.');
  if (dot_position != std::string::npos) {
    std::string extension = filename.substr(dot_position+1);
    if (extension == "frag") {
      return gl::kFragmentShader;
    } else if (extension == "vert") {
      return gl::kVertexShader;
    } else if (extension == "geom") {
      return gl::kGeometryShader;
    } else {
      throw std::invalid_argument("Can't guess the shader type of " + filename);
    }
  } else {
    throw std::invalid_argument("Can't guess the shader type of " + filename);
  }
}

class ShaderFile;
class ShaderProgram;
class ShaderManager {
  std::map<std::string, std::unique_ptr<ShaderFile>> shaders_;
  template<typename... Args>
  ShaderFile* load(Args&&... args);
 public:
  ShaderFile* publish(const std::string& filename, const gl::ShaderSource& src);
  ShaderFile* get(const std::string& filename);
};

class ShaderFile : public gl::Shader {
 public:
  ShaderFile(const std::string& filename)
      : ShaderFile(filename, gl::ShaderSource{filename}) {}

  ShaderFile(const std::string& filename, const gl::ShaderSource& src)
      : gl::Shader(shader_type(filename)) {
    std::string src_str = src.source();
    findIncludes(src_str);
    for (ShaderFile *included : includes_) {
      if (included->state_ == gl::Shader::kCompileFailure) {
        state_ = gl::Shader::kCompileFailure;
        return;
      }
    }
    findExports(src_str);
    set_source(src_str);
    set_source_file(filename);
    compile();
  }

  void set_update_func(std::function<void(const gl::Program&)> func) {
    update_func_ = func;
  }

  void update(const gl::Program& prog) const {
    if (update_func_) {
      update_func_(prog);
    }
  }

  const std::string& exports() const { return exports_; }

 private:
  std::function<void(const gl::Program&)> update_func_;
  std::vector<ShaderFile*> includes_;
  std::string exports_;

  void findExports(std::string &src);
  void findIncludes(std::string &src);

  friend class ShaderProgram;
};

class ShaderProgram : public gl::Program {
 public:
  ShaderProgram() {}

  template <typename... Shaders>
  explicit ShaderProgram(ShaderFile *shader, Shaders&&... shaders) {
    attachShaders(shader, shaders...);
    link();
  }

  ShaderProgram(const ShaderProgram& prog) = default;
  ShaderProgram(ShaderProgram&& prog) = default;

  void update() const {
    for (auto shader : shaders_) {
      shader->update(*this);
    }
  }

  template<typename... Rest>
  ShaderProgram& attachShaders(ShaderFile *shader, Rest&&... rest) {
    attachShader(shader);
    attachShaders(rest...);
    return *this;
  }
  ShaderProgram& attachShaders() {
    return *this;
  }

  // Depth First Search for all the included files, recursively
  ShaderProgram& attachShader(ShaderFile *shader) {
    if (shaders_.insert(shader).second) {
      for (auto include : shader->includes_) {
        attachShader(include);
      }
    }
    return *this;
  }

  virtual const Program& link() override {
    for (auto shader_file : shaders_) {
      const gl::Shader& shader = *shader_file;
      gl::Program::attachShader(shader);
    }
    gl::Program::link();

    return *this;
  }

 private:
  std::set<ShaderFile*> shaders_;
};

}  // namespace engine

#include "./shader_manager-inl.h"

#endif

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
  std::string extension = filename.substr(filename.size()-4, 4);
  if (extension == "frag") {
    return gl::kFragmentShader;
  } else if (extension == "vert") {
    return gl::kVertexShader;
  } else if (extension == "geom") {
    return gl::kGeometryShader;
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

class ShaderFile {
 public:
  ShaderFile(const std::string& filename)
      : shader_(shader_type(filename)), filename_(filename), src_(filename) {
    findExports();
    findIncludes();
  }

  ShaderFile(const std::string& filename, const gl::ShaderSource& src)
      : shader_(shader_type(filename)), filename_(filename), src_(src) {
    findExports();
    findIncludes();
  }

  void compile() {
    if (!shader_.compiled()) {
      shader_.set_source(src_);
      shader_.compile();
    }
  }

  void set_update_func(std::function<void(const gl::Program&)> func) {
    update_func_ = func;
  }

  void update(const gl::Program& prog) const {
    if (update_func_) {
      update_func_(prog);
    }
  }

  const std::string& filename() const { return filename_; }

  // Be careful with this: it won't effect the shader after it's compiled
  gl::ShaderSource& source() { return src_; }
  const gl::ShaderSource& source() const { return src_; }

  const gl::Shader& shader() {
    compile();
    return shader_;
  }

  const std::string& exports() const { return exports_; }

 private:
  gl::Shader shader_;
  std::string filename_;
  gl::ShaderSource src_;

  std::function<void(const gl::Program&)> update_func_;

  std::vector<ShaderFile*> includes_;
  std::string exports_;

  void findExports();
  void findIncludes();

  friend class ShaderProgram;
};

class ShaderProgram : public gl::Program {
 public:
  ShaderProgram(ShaderFile *vs, ShaderFile *fs) {
    getIncludes(vs);
    if (shaders_.find(fs) == shaders_.end()) { getIncludes(fs); }
    link();
  }

  ShaderProgram(ShaderFile *vs, ShaderFile *gs, ShaderFile *fs) {
    getIncludes(vs);
    if (shaders_.find(gs) == shaders_.end()) { getIncludes(gs); }
    if (shaders_.find(fs) == shaders_.end()) { getIncludes(fs); }
    link();
  }

  ShaderProgram(const ShaderProgram& prog) = default;
  ShaderProgram(ShaderProgram&& prog) : shaders_(prog.shaders_) { }

  void update() const {
    for (auto shader : shaders_) {
      shader->update(*this);
    }
  }

 private:
  std::set<ShaderFile*> shaders_;

  // Depth First Search for all the included files, recursively
  void getIncludes(ShaderFile *shader) {
    shaders_.insert(shader);
    for (auto include : shader->includes_) {
      if (shaders_.find(include) == shaders_.end()) {
        getIncludes(include);
      }
    }
  }

  void link() {
    for (auto shader_file : shaders_) {
      attachShader(shader_file->shader());
    }

    gl::Program::link();
  }
};

}  // namespace engine

#include "./shader_manager-inl.h"

#endif

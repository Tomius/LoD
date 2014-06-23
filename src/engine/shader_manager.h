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
  ShaderFile(ShaderManager* shader_storage,
             const std::string& filename)
      : shader_(shader_type(filename)), filename_(filename)
      , src_(filename), shader_storage_(shader_storage) {
    findExports();
    findIncludes();
  }

  ShaderFile(ShaderManager* shader_storage, const std::string& filename,
             const gl::ShaderSource& src)
      : shader_(shader_type(filename)), filename_(filename)
      , src_(src), shader_storage_(shader_storage) {
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

  ShaderManager* shader_storage_;
  std::function<void(const gl::Program&)> update_func_;

  std::vector<ShaderFile*> includes_;
  std::string exports_;

  void findExports() {
    // search for the exported functions
    std::string& str = src_.source();
    size_t export_pos = str.find("#export");
    while (export_pos != std::string::npos) {
      size_t line_end = str.find("\n", export_pos);
      size_t start_pos =
          str.find_first_not_of(" \t", export_pos + sizeof("#export") - 1);

      // Add the exported entity to exports_. The exports musn't be separated
      // with newline characters, it would mess up the line numbers, and the
      // GLSL error messages would be a lot harder to understand.
      exports_ += str.substr(start_pos, line_end - start_pos);

      // remove the export line (but leave the \n)
      str.erase(export_pos, line_end - export_pos);

      // Search for the next #export
      export_pos = str.find("#export");
    }
  }

  void findIncludes() {
    // look for #includes, search for the ShadingTechnique to add, then
    // call addToProgram on it. After that replace the include line
    // with the declarations of the functions, the tech provides.
    std::string& str = src_.source();
    size_t include_pos = str.find("#include");
    while (include_pos != std::string::npos) {
      size_t line_end = str.find("\n", include_pos);
      size_t start_comma = str.find("\"", include_pos);
      size_t end_comma = str.find("\"", start_comma+1);

      std::string included_filename =
          str.substr(start_comma+1, end_comma-start_comma-1);

      ShaderFile *included_shader = shader_storage_->get(included_filename);
      includes_.push_back(included_shader);

      // Replace the include directive with the included statements
      str.replace(include_pos, line_end - include_pos,
                  included_shader->exports());

      // Search for the next #include
      include_pos = str.find("#include");
    }
  }

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

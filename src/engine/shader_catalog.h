// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_SHADER_CATALOG_H_
#define ENGINE_SHADER_CATALOG_H_

#include <map>
#include <string>
#include <vector>

#include "./oglwrap_config.h"
#include "../oglwrap/shader.h"
#include "./scene.h"

namespace engine {

gl::ShaderType shader_type(const std::string& filename) {
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
class ShaderStore {
 public:
  static ShaderFile* get(const std::string& filename);
};

class ShaderFile {
  std::string filename_;
  gl::ShaderSource src_;
  gl::Shader shader_;

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
    gl::ShaderSource src {filename_};
    std::string& str = src.source();
    size_t include_pos = str.find("#include");
    while (include_pos != std::string::npos) {
      size_t line_end = str.find("\n", include_pos);
      size_t start_comma = str.find("\"", include_pos);
      size_t end_comma = str.find("\"", start_comma+1);

      std::string included_filename =
          str.substr(start_comma+1, end_comma-start_comma-1);

      ShaderFile *included_shader = ShaderStore::get(included_filename);
      includes_.push_back(included_shader);

      // Replace the include directive with the included statements
      str.replace(include_pos, line_end - include_pos,
                  included_shader->exports());

      // Search for the next #include
      include_pos = str.find("#include");
    }
  }

 public:
  explicit ShaderFile(const std::string& filename)
      : filename_(filename), src_(filename), shader_(shader_type(filename)) {
    findExports();
    findIncludes();
  }

  std::string& exports() { return exports_; }
  const std::string& exports() const { return exports_; }
};

class ShadingTechnique {
  std::string filename_;
  gl::ShaderSource src_;
  gl::Shader shader_;

 protected:
  std::vector<gl::Program*> programs_;  // The programs that use this tech
  std::string exports_;  // exported functons and variables

 public:
  explicit ShadingTechnique(const std::string& filename)
      : filename_(filename), src_(filename), shader_(shader_type(filename)) {
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
  virtual ~ShadingTechnique() {}

  virtual void addToProgram(gl::Program* prog) {
    programs_.push_back(prog);
    prog->attachShader(shader_);
  };
  virtual void update(const Scene& scene) {}

  std::string& src() { return src_.source(); }
  const std::string& src() const { return src_.source(); }
  std::string& exports() { return exports_; }
  const std::string& exports() const { return exports_; }
  const std::string& filename() const { return filename_; }
};

class ShaderCatalog {
  std::map<std::string, ShadingTechnique*> techs_;

 public:
  ShaderCatalog() = default;

  void publish(ShadingTechnique* technique) {
    techs_[technique->filename()] = technique;
  }

  gl::ShaderSource resolveIncludes(const std::string& filename,
                                   gl::Program* program) {
    // look for #includes, search for the ShadingTechnique to add, then
    // call addToProgram on it. After that replace the include line
    // with the declarations of the functions, the tech provides.
    gl::ShaderSource src {filename};
    std::string& str = src.source();
    size_t include_pos = str.find("#include");
    while (include_pos != std::string::npos) {
      size_t line_end = str.find("\n", include_pos);
      size_t start_comma = str.find("\"", include_pos);
      size_t end_comma = str.find("\"", start_comma+1);

      std::string included_tech =
          str.substr(start_comma+1, end_comma-start_comma-1);

      // fixme: search first
      ShadingTechnique *tech = techs_[included_tech];
      tech->addToProgram(program);

      // Replace the include directive with the included statements
      str.replace(include_pos, line_end - include_pos, tech->exports());

      // Search for the next #include
      include_pos = str.find("#include");
    }

    return src;
  }

  void update(const Scene& scene) const {
    for (auto& technique : techs_) {
      technique.second->update(scene);
    }
  }
};

}  // namespace engine

#endif

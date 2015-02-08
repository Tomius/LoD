#include "./shader_manager.h"
#include "./game_engine.h"

namespace engine {

void ShaderFile::findExports() {
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

void ShaderFile::findIncludes() {
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

    ShaderFile *included_shader =
      GameEngine::shader_manager()->get(included_filename);
    includes_.push_back(included_shader);

    // Replace the include directive with the included statements
    str.replace(include_pos, line_end - include_pos,
                included_shader->exports());

    // Search for the next #include
    include_pos = str.find("#include");
  }
}

}  // namespace engine

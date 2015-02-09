#include "./shader_manager.h"
#include "./game_engine.h"

namespace engine {

void ShaderFile::findExports(std::string &src) {
  // search for the exported functions
  size_t export_pos = src.find("#export");
  while (export_pos != std::string::npos) {
    size_t line_end = src.find("\n", export_pos);
    size_t start_pos =
        src.find_first_not_of(" \t", export_pos + sizeof("#export") - 1);

    // Add the exported entity to exports_. The exports musn't be separated
    // with newline characters, it would mess up the line numbers, and the
    // GLSL error messages would be a lot harder to understand.
    exports_ += src.substr(start_pos, line_end - start_pos);

    // remove the export line (but leave the \n)
    src.erase(export_pos, line_end - export_pos);

    // Search for the next #export
    export_pos = src.find("#export");
  }
}

void ShaderFile::findIncludes(std::string &src) {
  // look for #includes, search for the ShadingTechnique to add, then
  // call addToProgram on it. After that replace the include line
  // with the declarations of the functions, the tech provides.
  size_t include_pos = src.find("#include");
  while (include_pos != std::string::npos) {
    size_t line_end = src.find("\n", include_pos);
    size_t start_comma = src.find("\"", include_pos);
    size_t end_comma = src.find("\"", start_comma+1);

    std::string included_filename =
        src.substr(start_comma+1, end_comma-start_comma-1);

    ShaderFile *included_shader =
      GameEngine::shader_manager()->get(included_filename);
    includes_.push_back(included_shader);

    // Replace the include directive with the included statements
    src.replace(include_pos, line_end - include_pos,
                included_shader->exports());

    // Search for the next #include
    include_pos = src.find("#include");
  }
}

}  // namespace engine

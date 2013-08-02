#ifndef HEADER_40495872378D39A9
#define HEADER_40495872378D39A9

#include <string>
#include <stdexcept>
#include <oglplus/gl.hpp>
#include <oglplus/bound/texture.hpp>

std::string File2Str(const std::string& file);
void LoadTexture(const std::string& filename,
                 oglplus::Texture::Target target = oglplus::Texture::Target::_2D);

#endif // header guard

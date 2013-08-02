#include "misc.h"
#include <string>
#include <fstream>
#include <sstream>

// some miscellaneous (misc) code snippet

std::string File2Str(const std::string& file) {
    std::ifstream shaderFile(file.c_str());
    if(!shaderFile.is_open())
        shaderFile.open("shaders/" + file);
    if(!shaderFile.is_open())
        throw std::runtime_error("File: " + file + " not found.");
    std::stringstream shaderString;
    shaderString << shaderFile.rdbuf();
    return shaderString.str();
}

#include <iostream>
#include <ImageMagick/Magick++.h>
void LoadTexture(const std::string& filename, oglplus::Texture::Target target) {
    using namespace oglplus;
    try {
        Magick::Image image = Magick::Image(filename);
        Magick::Blob blob;
        image.write(&blob, "RGB");

        Texture::Image2D(
            target,
            0,
            PixelDataInternalFormat::CompressedSRGB,
            image.columns(),
            image.rows(),
            0,
            PixelDataFormat::RGB,
            PixelDataType::UnsignedByte,
            blob.data()
        );
    } catch(Magick::Error& Error) {
        Texture::Image2D(
            target, 0, PixelDataInternalFormat::RGBA, 1, 1, 0,
            PixelDataFormat::RGBA, PixelDataType::UnsignedByte, nullptr
        );
        Texture::BorderColor(target, Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
        Texture::WrapS(target, TextureWrap::ClampToBorder);
        Texture::WrapT(target, TextureWrap::ClampToBorder);
        Texture::WrapR(target, TextureWrap::ClampToBorder);

        std::cerr << "Error loading texture: " << Error.what() << std::endl;
    }
}

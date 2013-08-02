#include "terrainData.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <ImageMagick/Magick++.h>

using namespace std;

// -------======{[ RawImageData ]}======-------

RawImageData::RawImageData(const std::string& filename, const std::string& colorString) {
    if(filename.find(".rtc") != string::npos) {
        int num = 1;
        assert(*(char *)&num == 1);

        ifstream ifs(filename, ios::binary);
        ifs.read((char *)&w, sizeof(size_t));
        ifs.read((char *)&h, sizeof(size_t));
        ifs.read((char *)&bpp, sizeof(size_t));
        data.resize(w * h * bpp);
        ifs.read((char *)data.data(), data.size());
    } else {
        Magick::Image image(filename);
        w = image.columns();
        h = image.rows();
        bpp = colorString.length(); // BytesPerPixel
        data.resize(w * h * bpp);
        image.write(0, 0, w, h, colorString, MagickCore::CharPixel, data.data());
    }
}

void RawImageData::Save(const std::string& filename) {
    ofstream ofs(filename, ios::binary);
    ofs.write((const char *)&w, sizeof(size_t));
    ofs.write((const char *)&h, sizeof(size_t));
    ofs.write((const char *)&bpp, sizeof(size_t));
    ofs.write((const char *)data.data(), data.size());
}

// -------======{[ RawTerrainData ]}======-------

RawTerrainData::RawTerrainData(const std::string& filename) {
    std::cout << "Reading raw terrain data." << std::endl;
    if(filename.find(".rtd") != string::npos) {
        InitFromRawTerrain(filename);
    } else if(filename.find(".asc") != string::npos) {
        InitFromAsc(filename);
    } else {
        InitFromImage(filename);
    }
}

void RawTerrainData::Save(const std::string& filename) {
    ofstream ofs(filename, ios::binary);

    ofs.write((const char *)&w, sizeof(size_t));
    ofs.write((const char *)&h, sizeof(size_t));
    ofs.write((const char *)heightData.data(), heightData.size());

    assert(ofs.good());
}

void RawTerrainData::InitFromRawTerrain(const std::string& filename) {
    ifstream ifs(filename, ios::binary);
    if(!ifs.good()) {
        throw std::runtime_error("Error reading file: " + filename);
    }

    // FIXME: Endianness
    int num = 1;
    assert(*(char *)&num == 1);

    ifs.read((char *)&w, sizeof(size_t));
    ifs.read((char *)&h, sizeof(size_t));

    heightData.resize(w * h);
    ifs.read((char *)heightData.data(), heightData.size());

    if(!ifs.good()) {
        throw std::runtime_error("Error reading file: " + filename);
    }
}

void RawTerrainData::InitFromAsc(const std::string& filename) {
    ifstream ifs(filename);
    if(!ifs.good()) {
        throw std::runtime_error("Error reading file: " + filename);
    }
    string str;
    unsigned short iData;
    double dData;

    ifs >> str >> iData;
    assert(str == "ncols");
    w = iData;

    ifs >> str >> iData;
    assert(str == "nrows");
    h = iData;

    // Some data I don't need
    ifs >> str >> dData;
    assert(str == "xllcorner");
    ifs >> str >> dData;
    assert(str == "yllcorner");
    ifs >> str >> dData;
    assert(str == "cellsize");

    ifs >> str >> iData;
    assert(str == "NODATA_value");
    int noData = iData;

    std::vector<unsigned short> ushort_heightData;
    ushort_heightData.reserve(w * h);
    unsigned short max = 0;
    for(size_t i = 0; i < w * h; i++) {
        ifs >> iData;
        assert(iData != noData);
        ushort_heightData.push_back(iData);
        if(iData > max) {
            max = iData;
        }
    }

    heightData.reserve(w * h);
    for(size_t i = 0; i < w * h; i++) {
        unsigned char c = ushort_heightData[i] * 255. / max;
        heightData.push_back(c);
    }
}

void RawTerrainData::InitFromImage(const std::string& filename) {
    Magick::Image image(filename);
    w = image.columns();
    h = image.rows();
    heightData.resize(w * h);
    image.write(0, 0, w, h, "R", MagickCore::CharPixel, heightData.data());
}

void RawTerrainData::Convert(const std::string& filename, float xzScale, float yScale) {

    std::cout << "Preparing raw data for the use." << std::endl;
    int percent = 0;
    int currPercent = 0;

    // A utility macro to access height data ClampToEdge mode:
    #define height(_x, _y) heightData\
    [ std::min(std::max(_y, size_t(0)), h - 1) * w \
    + std::min(std::max(_x, size_t(0)), w - 1)]

    // Count the normals directly from the heightmap. I only need to
    // get the slope between the nearby texcoords to get a good
    // approximation what the actual smooth normal would be.
    std::vector<cvec3> normalData;
    normalData.reserve(w * h);
    for(size_t y = 0; y < h; y++) {
        for(size_t x = 0; x < w; x++) {

            float sx = height(x+1, y) - height(x-1, y);
            if(x == 0 || x == w - 1) {
                sx *= 2;
            }

            float sy = height(x, y+1) - height(x, y-1);
            if(y == 0 || y == h - 1) {
                sy *= 2;
            }

            float sxy = height(x+1, y+1) - height(x-1, y-1);
            if((y == 0 && x == 0) || (y == h - 1 && x == w - 1)) {
                sxy *= 2;
            }

            float syx = height(x+1, y-1) - height(x-1, y+1);
            if((y == 0 && x == w - 1) || (y == h - 1 && x == 0)) {
                syx *= 2;
            }

            vec3 normal = vec3(
                (sx + sxy + syx) * yScale,
                4 * xzScale,
               -1 * (sy + sxy - syx) * yScale // remember at ogl textures the first row is the bottom one.
            );

            normal.Normalize();
            normalData.push_back(cvec3(normal));
        }
        currPercent = y * w * 100 / (w * h);
        if(currPercent > percent) {
            std::cout << currPercent << '%' << std::endl;
            percent = currPercent;
        }
    }

    std::cout << "Preparation complete. Now saving to file." << std::endl;

    ofstream ofs(filename, ios::binary);

    ofs.write((const char *)&w, sizeof(size_t));
    ofs.write((const char *)&h, sizeof(size_t));
    ofs.write((const char *)&xzScale, sizeof(float));
    ofs.write((const char *)&yScale, sizeof(float));

    ofs.write((const char *)heightData.data(), heightData.size() * sizeof(char));
    ofs.write((const char *)normalData.data(), normalData.size() * sizeof(cvec3));

    assert(ofs.good());

    std::cout << "The data is built without an error and is ready to be rendered!" << std::endl;
    std::cout << "Now cleaning up." << std::endl;
}

// -------======{[ TerrainData ]}======-------

TerrainData::TerrainData(const std::string& datafile) {

    ifstream ifs(datafile, ios::binary);

    assert(datafile.find(".terrain") != string::npos);
    if(!ifs.good()) {
        throw std::runtime_error("Error reading file: " + datafile);
    }

    // FIXME
    int num = 1;
    assert(*(char *)&num == 1);

    ifs.read((char *)&w, sizeof(size_t));
    ifs.read((char *)&h, sizeof(size_t));
    ifs.read((char *)&xzScale, sizeof(float));
    ifs.read((char *)&yScale, sizeof(float));

    heightData.resize(w * h);
    normalData.resize(w * h);
    ifs.read((char *)heightData.data(), heightData.size());
    ifs.read((char *)normalData.data(), normalData.size() * sizeof(cvec3));

    assert(ifs.good());
}

// -------======{[ Math Utilities ]}======-------

vec3::vec3(float x, float y, float z)
    : x(x), y(y), z(z) {}
void vec3::Normalize() {
    float l = sqrt(x*x + y*y + z*z);
    if(l <= 1e-5)
        y = 1;
    else {
        x /= l;
        y /= l;
        z /= l;
    }
}

cvec3::cvec3()
    : x(0), y(0), z(0) {}

cvec3::cvec3(char x, char y, char z)
    : x(x), y(y), z(z) {}
cvec3::cvec3(vec3& rhs) {
    rhs.Normalize();
    x = rhs.x * 127;
    y = rhs.y * 127;
    z = rhs.z * 127;
}

ivec2::ivec2()
    : x(0), y(0) {}

ivec2::ivec2(int a, int b)
    : x(a), y(b) {}



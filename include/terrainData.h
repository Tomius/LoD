#ifndef HEADER_2D44B4B4C7E2D073
#define HEADER_2D44B4B4C7E2D073

#include <string>
#include <fstream>
#include <vector>

struct cvec3;
struct ivec2;
struct ivec3;

class RawImageData {
public:
    size_t w, h, bpp;
    std::vector<unsigned char> data;

    RawImageData(const std::string& filename, const std::string& colorString = "RGB");
    void Save(const std::string& filename);
};

class RawTerrainData {
    void InitFromRawTerrain(const std::string& filename);
    void InitFromAsc(const std::string& filename);
    void InitFromImage(const std::string& filename);
public:
    size_t w, h;
    std::vector<unsigned char> heightData;
    RawTerrainData(const std::string& filename);
    void Save(const std::string& filename);
    void Convert(const std::string& filename, float xzScale = 1.0f, float yScale = 1.0f);
};

class TerrainData {
public:
    float xzScale, yScale;
    size_t w, h;
    std::vector<unsigned char> heightData;
    std::vector<cvec3> normalData;

    TerrainData(const std::string& datafile);
};

struct vec3 {
    float x, y, z;
    vec3(float x, float y, float z);
    void Normalize();
};

struct cvec3 {
    char x, y, z;
    cvec3();
    cvec3(char x, char y, char z);
    cvec3(vec3& rhs);
};

struct ivec2 {
    int x, y;
    ivec2();
    ivec2(int a, int b);
};


#endif // header guard

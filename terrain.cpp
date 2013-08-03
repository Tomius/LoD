#include "terrain.h"
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

using namespace oglplus;
#define RESTART 0xFFFFFFFF

/* We want concentric rings made of hexagons like this, with increasing
   distance between two rings. This makes a great LOD effect if the viewer
   is near the center.

                            o-------o-------o
                           / \     / \     / \
                          /   \   /   \   /   \
                         /     \ /     \ /     \
                        o-------o-------o-------o
                       / \     / \     / \     / \
                      /   \   /   \   /   \   /   \
                     /     \ /     \ /     \ /     \
                    o-------o-------X-------o-------o
                     \     / \     / \     / \     /
                      \   /   \   /   \   /   \   /
                       \ /     \ /     \ /     \ /
                        o-------o-------o ------o
                         \     / \     / \     /
                          \   /   \   /   \   /
                           \ /     \ /     \ /
                            o-------o-------o

   Getting the coordinates for this in the normal XY coordinates can be tricky.
   Though you might notice that this is actually a very symmetrical fractal.
   Each ring is kinda alike of the previous one, just with more segments.
   In a pattern like this:
        n-th ring -> 6 line, starting at 1 o' clock - n point / line. (the end point counts at the next line)
   We define the points in a ring - line - segment coordinate system,
   where the 0th line is at 1 o' clock, and lines and segments increment clock-wise.
   Every "coordinate" starts from 0 not 1. Working in this coordinate system gets
   easier with these utility functions: */

static inline Vec2f GetPos(int ring, char line, int segment, float distance = 1.0f) {
#define sin60 0.866025404f
#define cos60 0.5f
    Vec2f points[] = {
        {cos60, sin60}, {1.0f, 0.0f}, {cos60, -sin60},
        {-cos60, -sin60}, {-1.0f, 0.0f}, {-cos60, sin60}
    };
    Vec2f prevPoint = points[size_t(line)]; // it's size_t to avoid compiler warning.
    Vec2f nextPoint = points[size_t((line + 1) % 6)];

    return distance * (
               (segment / (float)ring) * nextPoint +
               ((ring - segment) / (float)ring) * prevPoint
           );
}

static inline int GetIdx(int ring, char line, int segment) {
    if(ring == 0) {
        return 0;
    }
    // Count of vertices in full rings: 1 + 1*6 + 2*6 + ... + (ring - 1)*6
    int smallerRings = 1 + (ring - 1) * (ring) / 2 * 6;
    int currentRing = (line % 6) * ring + segment;
    return smallerRings + currentRing;
}

static inline void distIncr(int mmlev, float& distance, int ring) {
    distance += 1 << mmlev;
}

void PrintTime(const std::string& text = "") {
    static sf::Clock clock;
    static float lastTime = 0.0f;
    if(!text.empty()) {
        std::cout << text << ": " << clock.getElapsedTime().asSeconds() - lastTime << std::endl;
    }
    lastTime = clock.getElapsedTime().asSeconds();
}

Terrain::Terrain(const std::string& terrainFile,
                 const std::string& diffusePicture)
    : terrain(terrainFile), w(terrain.w), h(terrain.h), image(diffusePicture) {

    for(int m = 0; m < mmLev; m++) {
        const unsigned short ringCount = blockSize / (1 << m) + 1;

        vao[m].Bind();

        positions[m].Bind(Buffer::Target::Array);
        {
            std::vector<oglplus::Vec2f> verticesVector;
            verticesVector.push_back(Vec2f(0.0f, 0.0f));
            float distance = 0.0f;
            distIncr(m, distance, 0);
            for(int ring = 1; ring < ringCount; ring++) {
                for(char line = 0; line < 6; line++) {
                    for(int segment = 0; segment < ring; segment++) {
                        verticesVector.push_back(GetPos(ring, line, segment, distance));
                    }
                }
                distIncr(m, distance, ring);
            }

            vertexNum[m] = verticesVector.size();
            Buffer::Data(Buffer::Target::Array, verticesVector);
            VertexAttribArray(0).Setup<Vec2f>().Enable();
        }

        indices[m].Bind(Buffer::Target::ElementArray);
        {
            std::vector<unsigned int> indicesVector;
            for(int ring = 1; ring < ringCount; ring++) {
                for(char line = 0; line < 6; line++) {
                    for(int segment = 0; segment < ring - 1; segment++) {
                        indicesVector.push_back(GetIdx(ring, line, segment));
                        indicesVector.push_back(GetIdx(ring - 1, line, segment));
                    }
                    indicesVector.push_back(GetIdx(ring, line, ring - 1));
                    indicesVector.push_back(GetIdx(ring - 1, line + 1, 0));
                    indicesVector.push_back(GetIdx(ring, line + 1, 0));
                    indicesVector.push_back(RESTART);
                }
            }

            indexNum[m] = indicesVector.size();
            Buffer::Data(Buffer::Target::ElementArray, indicesVector);
        }
    }


    // Upload the textures:
    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

    Texture::Active(0);
    heightMap.Bind(Texture::Target::_2D);
    {
        Texture::Image2D(
            Texture::Target::_2D,
            0,
            PixelDataInternalFormat::R8,
            terrain.w,
            terrain.h,
            0,
            PixelDataFormat::Red,
            PixelDataType::UnsignedByte,
            terrain.heightData.data()
        );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        Texture::MinFilter(Texture::Target::_2D, TextureMinFilter::Linear);
        Texture::MagFilter(Texture::Target::_2D, TextureMagFilter::Linear);
        Texture::WrapS(Texture::Target::_2D, TextureWrap::Repeat);
    }

    Texture::Active(1);
    normalMap.Bind(Texture::Target::_2D);
    {
        Texture::Image2D(
            Texture::Target::_2D,
            0,
            PixelDataInternalFormat::RGB8,
            terrain.w,
            terrain.h,
            0,
            PixelDataFormat::RGB,
            PixelDataType::Byte,
            terrain.normalData.data()
        );

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        Texture::MinFilter(Texture::Target::_2D, TextureMinFilter::Linear);
        Texture::MagFilter(Texture::Target::_2D, TextureMagFilter::Linear);
        Texture::WrapS(Texture::Target::_2D, TextureWrap::Repeat);
    }

    Texture::Active(2);
    colorMap.Bind(Texture::Target::_2D);
    {
        Texture::Image2D(
            Texture::Target::_2D,
            0,
            PixelDataInternalFormat::SRGB8,
            image.w,
            image.h,
            0,
            PixelDataFormat::RGB,
            PixelDataType::UnsignedByte,
            image.data.data()
        );

        Texture::GenerateMipmap(Texture::Target::_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        Texture::MinFilter(Texture::Target::_2D, TextureMinFilter::LinearMipmapNearest);
        Texture::MagFilter(Texture::Target::_2D, TextureMagFilter::Linear);
        Texture::WrapS(Texture::Target::_2D, TextureWrap::Repeat);
    }

    VertexArray::Unbind();
}

// -------======{[ Functions about creating the map from the blocks ]}======-------

    /* A Hexagon is definitely needed here too to understand the code.
                   (at least for me, it helped a lot)

                            o-------o-------o
                           / \     / \     / \
                          /   \   /   \   /   \
                         /     \ /     \ /     \
                        o-------o-------o-------o
                       / \     / \     / \     / \
                      /   \   /   \   /   \   /   \
                     /     \ /     \ /     \ /     \
                    o-------o-------X-------o-------o
                     \     / \     / \     / \     /
                      \   /   \   /   \   /   \   /
                       \ /     \ /     \ /     \ /
                        o-------o-------o ------o
                         \     / \     / \     /
                          \   /   \   /   \   /
                           \ /     \ /     \ /
                            o-------o-------o                               */

static inline Vec2f GetBlockPos(int ring, char line, int segment, float distance = 1.0f) {
#define cos30 0.866025404f
#define sin30 0.5f
    // Actually the distance to up is cos30, the 1 distance is towards
    // the hexagon vertices. Even still it's easier to define a points
    // like this, and multiply the distance with cos30 rather than
    // multiplying every single point with cos30.
    Vec2f points[] = {
        {cos30, sin30}, {cos30, -sin30}, {0.0f, -1.0f},
        {-cos30, -sin30}, {-cos30, sin30}, {0.0f, 1.0},
    };
    Vec2f prevPoint = points[size_t(line)]; // it's size_t to avoid compiler warning.
    Vec2f nextPoint = points[size_t((line + 1) % 6)];

    return cos30 * distance * (
               (segment / (float)ring) * nextPoint +
               ((ring - segment) / (float)ring) * prevPoint
           );
}

static inline int GetBlockMipmapLevel(Vec2f pos, Vec2f camPos) {
    return std::min(
        std::max(
            int(log2(Length(pos - camPos)) - log2(2 * blockSize)),
            0
        ), mmLev - 1
    );
}

//static void GetLinePoints(std::vector<Vec2f>& data, int ringID,
//                          char lineID, size_t mLev, bool halfSegments = false) {
//    float distance = 0.0f;
//    for(int ring = 0; ring < ringID; ring++)
//        distIncr(mLev, distance, 0);
//    for(int segment = 0; segment < halfSegments ? ringID / 2 : ringID; segment++)
//        data.push_back(GetPos(ringID, lineID, segment, distance));
//}
//
//
//static inline void CreateConnectors(Vec2f& pos, Vec2f& camPos) {
//
//    int own_mipmap = GetBlockMipmapLevel(pos, camPos);
//    int neighbour_mipmaps[6];
//    for(int i = 0; i < 6; i++) {
//        Vec2f neighbour = GetBlockPos(1, i, 0, 2*blockSize);
//        neighbour_mipmaps[i] = GetBlockMipmapLevel(pos + neighbour, camPos);
//    }
//
//    std::vector<Vec2f> innerVertices, outerVertices;
//
//    int outerRing = blockSize / (1 << own_mipmap);
//    for(int i = 0; i < 6; i++) {
//
//        if(own_mipmap > neighbour_mipmaps[i]) {
//            GetLinePoints(innerVertices, outerRing - 1, i, own_mipmap);
//            GetLinePoints(outerVertices, outerRing, i, own_mipmap, true);
//        } else {
//            GetLinePoints(innerVertices, outerRing - 1, i, own_mipmap);
//            GetLinePoints(outerVertices, outerRing, i, own_mipmap);
//        }
//    }
//}

void Terrain::DrawBlocks(const oglplus::Vec3f& _camPos, oglplus::LazyProgramUniform<Vec2f>& Offset) {
    // The center piece is special
    Vec2f pos(0.0f, 0.0f);
    Offset = pos;
    Vec2f camPos = Vec2f(_camPos.x(), _camPos.z());
    int mipmapLevel = GetBlockMipmapLevel(pos, camPos);

    // Draw
    vao[mipmapLevel].Bind();
    gl.DrawElements(PrimitiveType::TriangleStrip, indexNum[mipmapLevel], DataType::UnsignedInt);

    // All the other ones
    float distance = 2 * blockSize;
    int ringCount = std::max(w, h) / (2 * blockSize) + 1;
    for(int ring = 1; ring < ringCount; ring++) {
        for(char line = 0; line < 6; line++) {
            for(int segment = 0; segment < ring ; segment++) {
                pos = GetBlockPos(ring, line, segment, distance);
                Offset = pos;
                mipmapLevel = GetBlockMipmapLevel(pos, camPos);

                // Draw
                vao[mipmapLevel].Bind();
                gl.DrawElements(PrimitiveType::TriangleStrip, indexNum[mipmapLevel], DataType::UnsignedInt);
            }
        }
        distance += 2 * blockSize;
    }
}

void Terrain::Render(const oglplus::Vec3f& camPos,
                     oglplus::LazyProgramUniform<oglplus::Vec2f>& Offset,
                     oglplus::LazyProgramUniform<oglplus::Vec3f>& scales) {

    // Logically, this should into Terrain constructor. However, the shader
    // program doesn't exist yet when the constructor runs, so I set this
    // uniform value at the first render call
    static bool firstExec = true;
    if(firstExec) {
        scales.Set(Vec3f(terrain.xzScale, terrain.yScale, terrain.xzScale));
        firstExec = false;
    }

    Texture::Active(0);
    heightMap.Bind(Texture::Target::_2D);
    Texture::Active(1);
    normalMap.Bind(Texture::Target::_2D);
    Texture::Active(2);
    colorMap.Bind(Texture::Target::_2D);
    gl.Enable(Capability::PrimitiveRestart);
    gl.PrimitiveRestartIndex(RESTART);

    //gl.PolygonMode(PolygonMode::Line);
    DrawBlocks(camPos, Offset);
    //gl.PolygonMode(PolygonMode::Fill);

    gl.Disable(Capability::PrimitiveRestart);
    VertexArray::Unbind();
}


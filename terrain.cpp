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
        const unsigned short ringCount = blockSize / (1 << m);

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
        {0.0f, 1.0}, {cos30, sin30}, {cos30, -sin30},
        {0.0f, -1.0f}, {-cos30, -sin30}, {-cos30, sin30},
    };
    Vec2f prevPoint = points[size_t(line)]; // it's size_t to avoid compiler warning.
    Vec2f nextPoint = points[size_t((line + 1) % 6)];

    return cos30 * distance * (
               (segment / (float)ring) * nextPoint +
               ((ring - segment) / (float)ring) * prevPoint
           );
}

static char getLineFromAngle(float angleInRadians) {
    float angleDegrees = fmod(angleInRadians * 180 / M_PI, 360);
    float baseAngle = 30;
    for(int i = 0; i < 6; i++) {
        if(fabs(angleDegrees - (baseAngle + i * 60)) < 5){
            return i;
        }
    }
    // Shouldn't get here.
    assert(0);
}

static void getLinePoints(std::vector<Vec2f>& data, int ringID, char lineID, size_t mLev) {
    float distance = 0.0f;
    for(int ring = 0; ring < ringID; ring++)
        distIncr(mLev, distance, 0);
    for(int segment = 0; segment < ringID; segment++)
        data.push_back(GetPos(ringId, lineID, segment, distance));
}

static inline void CreateConnector(oglplus::LazyProgramUniform<Vec2f>& Offset,
                                   Vec2f& center1, Vec2f& center2, size_t mLev1, size_t mLev2) {
    Vec2f dist = center2 - center1;
    float angle = atan2(dist.y(), dist.x());
    char line1 = getLineFromAngle(angle);
    char line2 = (line1 + 3) % 6;
    short outerRing1 = blockSize / (1 << mLev1);
    short outerRing2 = blockSize / (1 << mLev2);
    std::vector<Vec2f> innerPoints1, innerPoints2; // the (outerRing - 1)'s points
    std::vector<Vec2f> outerPoints; // we want both rings to use the smaller outer ring

    getLinePoints(innerPoints1, outerRing1 - 1, line1, mLev1);
    getLinePoints(innerPoints2, outerRing2 - 1, line2, mLev2);
    if(mLev1 > mLev2) { // higher mipmap level means smaller ring (smaller in point number).
        getLinePoints(outerPoints, outerRing1, line1, mLev1);
        // We have to offset the other points, because they
        // are at the opposite sides of the hexagon.
        for(int i = 0; i < innerPoints2.size(); i++)
            innerPoints2[i] += dist;
        // The Gpu will offset all 3 point vector with center1,
        // so they will all get at right place
        Offset = center1;
    } else { // do the exact same thing just replace the role of hexa1 and hexa2
        getLinePoints(outerPoints, outerRing2, line2, mLev2);
        for(int i = 0; i < innerPoints1.size(); i++)
            innerPoints1[i] += dist;
        Offset = center2;
    }

    VertexArray vao;
    vao.Bind();

    Buffer positions, indices;

    positions.Bind(Buffer::Target::Array) {
        Buffer::Data(Buffer::Target::Array, innerPoints1);
        Buffer::SubData(Buffer::Target::Array, innerPoints1.size(), outerPoints);
        Buffer::SubData(Buffer::Target::Array, innerPoints1.size() + outerPoints.size(), innerPoints2);
    }
}

void Terrain::DrawBlocks(const oglplus::Vec3f& _camPos, oglplus::LazyProgramUniform<Vec2f>& Offset) {
    // The center piece is special
    Vec2f pos(0.0f, 0.0f);
    Offset = pos;
    Vec2f camPos = Vec2f(_camPos.x(), _camPos.z());
    int mipmapLevel = std::min(
        std::max(
            int(log2(Length(pos - camPos)) - log2( 2 * blockSize)),
            0
        ), mmLev - 1
    );

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
                int mipmapLevel = std::min(
                    std::max(
                        int(log2(Length(pos - camPos)) - log2(2 * blockSize)),
                        0
                    ), mmLev - 1
                );

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


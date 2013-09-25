#include "terrainMesh.h"
#include "svec2.hpp"

using namespace oglwrap;
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

   Irregularness looks good with tessellation, but a normal hexagon is "too irregular".
   The problem with it, is that I assume that it's width is 1, it's height will be
   sqrt(3)/2, which isn't really a whole number, and the mesh can actually be pretty big,
   and floats are not precise. Instead we can use a bit stretched hexagon, so that it's
   width and height equals. With this we can use integral position coordinates, and
   short is likely to be enough(which is half the size of a float :) ). There's also
   another problem: The odd rows are shifted with a half texel compared to even rows.
   To handle this, work with twice as big hexagons, but will fix this in the vs.

   Getting the coordinates for this in the normal XY coordinates can be tricky.
   Though you might notice that this is actually a very symmetrical fractal.
   Each ring is kinda alike of the previous one, just with more segments.
   In a pattern like this:
        n-th ring -> 6 line, starting at 1 o' clock - n point / line. (the end point counts at the next line)
   We define the points in a ring - line - segment coordinate system,
   where the 0th line is at 1 o' clock, and lines and segments increment clock-wise.
   Every "coordinate" starts from 0 not 1. Working in this coordinate system gets
   easier with these utility functions: */

static inline svec2 GetPos(int ring, char line, int segment, int distance = 1) {
  /*
              5-------0
             / \     / \
            /   \   /   \
           /     \ /     \
          4-------X-------1
           \     / \     /
            \   /   \   /
             \ /     \ /
              3-------2
                                      */

  svec2 points[6];
  points[0] = svec2(1, 2);
  points[1] = svec2(2, 0);
  points[2] = svec2(1, -2);
  points[3] = svec2(-1, -2);
  points[4] = svec2(-2, 0);
  points[5] = svec2(-1, 2);

  svec2 prevPoint = distance * points[size_t(line % 6)]; // it's size_t to avoid compiler warning.
  svec2 nextPoint = distance * points[size_t((line + 1) % 6)];

  return prevPoint + svec2(
           // This could overflow if done with shorts, even if the result is a valid short
           int(nextPoint.x - prevPoint.x) * segment / ring,
           int(nextPoint.y - prevPoint.y) * segment / ring
         );

}

static inline int GetIdx(int ring, char _line, int _segment) {
  if(ring == 0) {
    return 0;
  }
  // Count of vertices in full rings: 1 + 1*6 + 2*6 + ... + (ring - 1)*6
  int smallerRings = 1 + (ring - 1) * (ring) / 2 * 6;
  // When counting the current ring, i also let the segment num loop to the next line
  // For example the second ring's 0th line's end point can be addressed as (2, 0, 2),
  // not just (2, 1, 0). This simplifies the way the indicies are counted by a lot.
  char line = _line;
  int segment = _segment;
  if(_segment >= ring) {
    segment = ring - _segment;
    line = _line + 1;
  }
  int currentRing = (line % 6) * ring + segment;
  return smallerRings + currentRing;
}

static inline void distIncr(int mmlev, int& distance) {
  distance += 1 << (mmlev + 1);
}

// Warning this function is nearly impossible to understand just from the code.
// Grab a paper and a pen, and draw simple figures about it, I mean draw 1-2 ring
// hexagons to understand the vertex positions part, and lines with max 6-8 segments
// to see what happens with the indices, and with that, the code will look trivial.
TerrainMesh::TerrainMesh(const std::string& terrainFile)
  : terrain(terrainFile), w(terrain.w), h(terrain.h) {

  for(int m = 0; m < blockMipmapLevel; m++) {
    const unsigned short ringCount = blockRadius / (1 << (m + 1)) + 1;

    vao[m].bind();

    positions[m].bind();
    {
      std::vector<svec2> verticesVector;
      verticesVector.push_back(svec2());
      int distance = 0;
      distIncr(m, distance);
      for(int ring = 1; ring < ringCount; ring++) {
        for(char line = 0; line < 6; line++) {
          for(int segment = 0; segment < ring; segment++) {
            verticesVector.push_back(
              GetPos(ring, line, segment, distance)
            );
          }
        }
        distIncr(m, distance);
      }

      vertexNum[m] = verticesVector.size();
      positions[m].data(verticesVector);
      VertexAttribArray(0).setup<short>(2).enable();
    }

    indices[m].bind();
    {
      std::vector<unsigned int> indicesVector;
      for(int ring = 1; ring < ringCount - 1; ring++) { // the border indices are separate
        for(char line = 0; line < 6; line++) {
          for(int segment = 0; segment < ring; segment++) {
            indicesVector.push_back(GetIdx(ring, line, segment));
            indicesVector.push_back(GetIdx(ring - 1, line, segment));
          }
          // There's one extra vertex at the end.
          indicesVector.push_back(GetIdx(ring, line + 1, 0));
          indicesVector.push_back(RESTART);
        }
      }

      indexNum[m] = indicesVector.size();
      indices[m].data(indicesVector);

    }

    // -------======{[ Create the border indices ]}======-------

    {
      int ring = ringCount - 1;

      // The normal ones
      for(int line = 0; line < 6; line++) {

        std::vector<unsigned int> indicesVector;

        for(int segment = 0; segment < ring; segment++) {
          indicesVector.push_back(GetIdx(ring, line, segment));
          indicesVector.push_back(GetIdx(ring - 1, line, segment));
        }
        // There's one extra vertex at the end.
        indicesVector.push_back(GetIdx(ring, line + 1, 0));
        indicesVector.push_back(RESTART);

        borderIndices[m][line][0].bind();
        borderIndices[m][line][0].data(indicesVector);
      }

      // The ones that connect different mipmapLevel blocks
      // Idea: skip every odd vertex on the outer ring
      for(int line = 0; line < 6; line++) {

        std::vector<unsigned int> indicesVector;

        // The first set
        for(int segment = 0; segment < ring; segment += 2) {
          if(segment != 0) {
            indicesVector.push_back(GetIdx(ring - 1, line, segment - 1));
          }
          indicesVector.push_back(GetIdx(ring, line, segment));
          indicesVector.push_back(GetIdx(ring - 1, line, segment));
          indicesVector.push_back(GetIdx(ring - 1, line, segment + 1));
          indicesVector.push_back(RESTART);
        }


        // The second set.
        for(int segment = 0; segment < ring; segment += 2) {
          indicesVector.push_back(GetIdx(ring, line, segment));
          indicesVector.push_back(GetIdx(ring - 1, line, segment + 1));
          indicesVector.push_back(GetIdx(ring, line, segment + 2));
          indicesVector.push_back(RESTART);
        }

        borderIndices[m][line][1].bind();
        borderIndices[m][line][1].data(indicesVector);
      }
    }

  }


  // Upload the textures:
  GLfloat maxAniso = 0.0f;
  gl(GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso));

  // Laziness level OVER 9000!!!
  gl(PixelStorei(GL_UNPACK_ALIGNMENT, 1));
  gl(PixelStorei(GL_PACK_ALIGNMENT, 1));

  heightMap.active(0);
  heightMap.bind();
  {
    heightMap.upload(
      PixelDataInternalFormat::R8,
      terrain.w,
      terrain.h,
      PixelDataFormat::Red,
      PixelDataType::UnsignedByte,
      terrain.heightData.data()
    );

    heightMap.anisotropy(maxAniso);
    heightMap.minFilter(MinFilter::Linear);
    heightMap.magFilter(MagFilter::Linear);
  }

  grassMaps[0].bind();
  {
    grassMaps[0].loadTexture("textures/grass.jpg");
    grassMaps[0].generateMipmap();
    grassMaps[0].minFilter(MinFilter::LinearMipmapLinear);
    grassMaps[0].magFilter(MagFilter::Linear);
    grassMaps[0].wrapS(Wrap::Repeat);
    grassMaps[0].wrapT(Wrap::Repeat);
  }

  grassMaps[1].bind();
  {
    grassMaps[1].loadTexture("textures/grass_2.jpg");
    grassMaps[1].generateMipmap();
    grassMaps[1].minFilter(MinFilter::LinearMipmapLinear);
    grassMaps[1].magFilter(MagFilter::Linear);
    grassMaps[1].wrapS(Wrap::Repeat);
    grassMaps[1].wrapT(Wrap::Repeat);
  }

  grassNormalMap.bind();
  {
    grassNormalMap.loadTexture("textures/grass_normal.jpg");
    grassNormalMap.generateMipmap();
    grassNormalMap.minFilter(MinFilter::LinearMipmapLinear);
    grassNormalMap.magFilter(MagFilter::Linear);
    grassNormalMap.wrapS(Wrap::Repeat);
    grassNormalMap.wrapT(Wrap::Repeat);
  }

  VertexArray::Unbind();
  Texture2D::Unbind();
}

// -------======{[ Functions about creating the map from the blocks ]}======-------

// This works with ivec2 rather than svec2, as it's result is
// uploaded as a uniform which have to an int anyway.
static inline glm::ivec2 GetBlockPos(int ring, char line, int segment, int distance = 1) {
  /*
                           o----- (5) -----o
                          / \             / \
                         /   \   /   \   /   \
                              \ /     \ /
                      (4) -----o-------o----- (0)
                        \     / \     / \
                     /   \   /   \   /   \   /   \
                    /     \ /     \ /     \ /     \
                   o-------o-------X-------o-------o
                    \     / \     / \     / \     /
                     \   /   \   /   \   /   \   /
                              \ /     \ /
                      (3) -----o-------o ---- (1)
                              / \     / \
                         \   /   \   /   \   /
                          \ /             \ /
                           o----- (2) -----o                               */

  glm::ivec2 points[6];
  points[0] = glm::ivec2(3, 2);
  points[1] = glm::ivec2(3, -2);
  points[2] = glm::ivec2(0, -4);
  points[3] = glm::ivec2(-3, -2);
  points[4] = glm::ivec2(-3, 2);
  points[5] = glm::ivec2(0, 4);

  glm::ivec2 prevPoint = distance * points[size_t(line % 6)]; // it's size_t to avoid compiler warning.
  glm::ivec2 nextPoint = distance * points[size_t((line + 1) % 6)];

  return prevPoint + (nextPoint - prevPoint) * segment / ring;
}

static inline int GetBlockMipmapLevel(glm::ivec2 _pos, glm::vec2 camPos) {
  glm::vec2 pos(_pos.x / 2, _pos.y / 2);

  return std::min(
           std::max(
             int(log2(glm::length(pos - camPos)) - log2(2 * blockRadius)),
             0
           ), blockMipmapLevel - 1
         );
}

void TerrainMesh::CreateConnectors(glm::ivec2 pos, glm::vec2 camPos) {

  int own_mipmap = GetBlockMipmapLevel(pos, camPos);
  int neighbour_mipmaps[6];
  for(int line = 0; line < 6; line++) {
    glm::ivec2 neighbour = GetBlockPos(1, line, 0, blockRadius);
    neighbour_mipmaps[line] = GetBlockMipmapLevel(pos + neighbour, camPos);
  }

  for(int line = 0; line < 6; line++) {
    int irregular = own_mipmap < neighbour_mipmaps[line] ? 1 : 0;

    borderIndices[own_mipmap][line][irregular].bind();
    size_t indicesNum = borderIndices[own_mipmap][line][irregular].size() / sizeof(int);

    gl(DrawElements(GL_TRIANGLE_STRIP, indicesNum, DataType::UnsignedInt, nullptr));
  }
}

void TerrainMesh::DrawBlocks(const glm::vec3& _camPos,
                             oglwrap::LazyUniform<glm::ivec2>& offset,
                             oglwrap::LazyUniform<int>& mipmapLevel_uniform) {

  // The center piece is special
  glm::ivec2 pos(0, 0);
  offset = pos;
  glm::vec2 camPos = glm::vec2(_camPos.x, _camPos.z);
  int mipmapLevel = GetBlockMipmapLevel(pos, camPos);
  mipmapLevel_uniform = mipmapLevel;

  // Draw
  vao[mipmapLevel].bind();
  indices[mipmapLevel].bind();
  gl(DrawElements(GL_TRIANGLE_STRIP, indexNum[mipmapLevel], DataType::UnsignedInt, nullptr));
  CreateConnectors(pos, camPos);


  // All the other ones
  int distance = blockRadius;
  int ringCount = std::max(w, h) / (2 * blockRadius) + 1;
  for(int ring = 1; ring < ringCount; ring++) {
    for(char line = 0; line < 6; line++) {
      for(int segment = 0; segment < ring ; segment++) {
        pos = GetBlockPos(ring, line, segment, distance);
        offset = pos;
        mipmapLevel = GetBlockMipmapLevel(pos, camPos);
        mipmapLevel_uniform = mipmapLevel;

        // Draw
        vao[mipmapLevel].bind();
        indices[mipmapLevel].bind();
        gl(DrawElements(GL_TRIANGLE_STRIP, indexNum[mipmapLevel], DataType::UnsignedInt, nullptr));
        CreateConnectors(pos, camPos);
      }
    }
    distance += blockRadius;
  }

}

void TerrainMesh::render(const glm::vec3& camPos,
                         oglwrap::LazyUniform<glm::ivec2>& offset,
                         oglwrap::LazyUniform<int>& mipmapLevel_uniform) {

  heightMap.active(0);
  heightMap.bind();
  grassMaps[0].active(1);
  grassMaps[0].bind();
  grassMaps[1].active(2);
  grassMaps[1].bind();
  grassNormalMap.active(3);
  grassNormalMap.bind();

  gl(Enable(GL_PRIMITIVE_RESTART));
  gl(PrimitiveRestartIndex(RESTART));
  //gl( PolygonMode(GL_FRONT_AND_BACK, GL_LINE) );

  DrawBlocks(camPos, offset, mipmapLevel_uniform);

  //gl( PolygonMode(GL_FRONT_AND_BACK, GL_FILL) );
  gl(Disable(GL_PRIMITIVE_RESTART));

  VertexArray::Unbind();
  grassNormalMap.active(3);
  grassNormalMap.unbind();
  grassMaps[1].active(2);
  grassMaps[1].unbind();
  grassMaps[0].active(1);
  grassMaps[0].unbind();
  heightMap.active(0);
  heightMap.unbind();
}

unsigned char TerrainMesh::fetchHeight(glm::ivec2 v) const {
  return terrain.heightData[(v.y + terrain.h/2) * terrain.w + (v.x + terrain.w/2)];
}

double TerrainMesh::getHeight(double x, double y) const {
  using namespace std;
  using namespace glm;
  ivec2 coord = ivec2(round(x), round(y));

  /*    Neighbors in geometry:

              5-------0         Neighbors in the texture:
             / \     / \
            /   \   /   \                5---0
           /     \ /     \               |   |
          4-------X-------1   ->         4---X---1
           \     / \     /               |   |
            \   /   \   /                3---2
             \ /     \ /
              3-------2
                                      */
  ivec2 neighbors[6];
  neighbors[0] = ivec2(0, +1);
  neighbors[1] = ivec2(+1,  0);
  neighbors[2] = ivec2(0, -1);
  neighbors[3] = ivec2(-1, -1);
  neighbors[4] = ivec2(-1,  0);
  neighbors[5] = ivec2(-1, +1);

  ivec2 geometry_neighbors[6];
  geometry_neighbors[0] = ivec2(+1, +2);
  geometry_neighbors[1] = ivec2(+2,  0);
  geometry_neighbors[2] = ivec2(+1, -2);
  geometry_neighbors[3] = ivec2(-1, -2);
  geometry_neighbors[4] = ivec2(-2,  0);
  geometry_neighbors[5] = ivec2(-1, +2);

  // Try all the 6 six triangle, and chose the one,
  // which generates valid barycentric co-ordinates.
  for(int i = 0; i < 6; i ++) {

    unsigned char neighbor_a = i;
    unsigned char neighbor_b = (i + 1) % 6;

    // Relative coordinates
    const ivec2 a_coord = neighbors[neighbor_a];
    const ivec2 b_coord = neighbors[neighbor_b];
    const ivec2 c_coord = ivec2(0.0);

    // Heights
    double a_height = fetchHeight(coord + a_coord);
    double b_height = fetchHeight(coord + b_coord);
    double c_height = fetchHeight(coord + c_coord);

    // Get the barycentric weights for this point
    // see: http://en.wikipedia.org/wiki/Barycentric_coordinate_system
    const ivec2 a_geom_coord = geometry_neighbors[neighbor_a];
    const ivec2 b_geom_coord = geometry_neighbors[neighbor_b];
    const ivec2 c_geom_coord = ivec2(0.0);

    dvec2 a = dvec2(a_geom_coord.x, a_geom_coord.y);
    dvec2 b = dvec2(b_geom_coord.x, b_geom_coord.y);
    dvec2 c = dvec2(c_geom_coord.x, c_geom_coord.y);

    double x_diff = 2 * (x - coord.x), y_diff = 2 * (y - coord.y);
    double lambda_a_nom = (b.y - c.y) * (x_diff - c.x) + (c.x - b.x) * (y_diff - c.y);
    double lambda_b_nom = (c.y - a.y) * (x_diff - c.x) + (a.x - c.x) * (y_diff - c.y);
    double lambda_denom = (b.y - c.y) * (a.x    - c.x) + (c.x - b.x) * (a.y    - c.y);

    double a_lambda = lambda_a_nom / lambda_denom;
    double b_lambda = lambda_b_nom / lambda_denom;
    double c_lambda = 1 - a_lambda - b_lambda;

    if(std::min(std::min(a_lambda, b_lambda), c_lambda) < 0.0) {
      continue;
    }

    return a_lambda * a_height +
           b_lambda * b_height +
           c_lambda * c_height;
  }

  // If all fails
  return fetchHeight(coord);
}



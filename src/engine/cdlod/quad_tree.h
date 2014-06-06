// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include "quad_grid_mesh.h"
#include "../camera.h"
#include "../collision/bounding_box.h"
#include "../height_map_interface.h"

namespace engine {

namespace cdlod {

class QuadTree {
  QuadGridMesh mesh_;
  GLubyte node_dimension_;

  struct Node {
    GLshort x, z;
    GLshort min_y, max_y;
    GLushort size;
    GLubyte level;
    std::unique_ptr<Node> tl, tr, bl, br;

    Node(GLshort x, GLshort z, GLubyte level, GLubyte dimension);

    BoundingBox boundingBox() const {
      return BoundingBox(glm::vec3(x-size/2, min_y, z-size/2),
                         glm::vec3(x+size/2, max_y, z+size/2));
    }

    bool collidesWithSphere(const glm::vec3& center, float radius) {
      return boundingBox().collidesWithSphere(center, radius);
    }

    svec2 countMinMaxOfArea(const HeightMapInterface& hmap);

    void selectNodes(const glm::vec3& cam_pos, const Frustum& frustum,
                     QuadGridMesh& grid_mesh, int node_dimension);
  };

  Node root_;

 public:
  QuadTree(const HeightMapInterface& hmap, int node_dimension = 128)
      : mesh_(node_dimension), node_dimension_(node_dimension)
      , root_(hmap.w()/2, hmap.h()/2,
        std::max(log2(std::max(hmap.w(), hmap.h())) - log2(node_dimension), 0.0),
        node_dimension) {
    root_.countMinMaxOfArea(hmap);
  }

  GLubyte node_dimension() const {
    return node_dimension_;
  }

  void setupPositions(gl::VertexAttribArray attrib) {
    mesh_.setupPositions(attrib);
  }

  // Uses vertex attrib divisor!
  void setupRenderData(gl::VertexAttribArray attrib) {
    mesh_.setupRenderData(attrib);
  }

  // render with vertex attrib divisor
  void render(const engine::Camera& cam) {
    mesh_.clearRenderList();
    root_.selectNodes(cam.pos(), cam.frustum(), mesh_, node_dimension_);
    mesh_.render();
  }

  // render with uniforms
  void render(const engine::Camera& cam,
              const gl::UniformObject<glm::vec4>& uRenderData) {
    mesh_.clearRenderList();
    root_.selectNodes(cam.pos(), cam.frustum(), mesh_, node_dimension_);
    mesh_.render(uRenderData);
  }
};

} // namespace cdlod

} // namespace engine

#endif

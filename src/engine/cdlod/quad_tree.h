// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include "./quad_grid_mesh.h"
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
    BoundingBox bbox;
    GLushort size;
    GLubyte level;
    std::unique_ptr<Node> tl, tr, bl, br;

    Node(GLshort x, GLshort z, GLubyte level, GLubyte dimension, bool root = false);

    // Helper to create the quadtree in four threads
    static void Init(GLshort x, GLshort z, GLubyte level,
                     GLubyte dimension, std::unique_ptr<Node>* node);

    bool collidesWithSphere(const glm::vec3& center, float radius) {
      return bbox.collidesWithSphere(center, radius);
    }

    void countMinMaxOfArea(const HeightMapInterface& hmap,
                           double *min, double *max, bool root = false);

    // Helper to run countMinMaxOfArea in thread
    static void CountMinMaxOfArea(Node* node, const HeightMapInterface& hmap,
                                  double *min, double *max) {
      node->countMinMaxOfArea(hmap, min, max);
    }

    void selectNodes(const glm::vec3& cam_pos, const Frustum& frustum,
                     QuadGridMesh& grid_mesh, int node_dimension);
  };

  Node root_;

 public:
  QuadTree(const HeightMapInterface& hmap, int node_dimension = 128)
      : mesh_(node_dimension), node_dimension_(node_dimension)
      , root_(hmap.w()/2, hmap.h()/2,
        std::max(log2(std::max(hmap.w()-1, hmap.h()-1)) - log2(node_dimension), 0.0),
        node_dimension, true) {
    double min, max;
    root_.countMinMaxOfArea(hmap, &min, &max, true);
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
    root_.selectNodes(cam.transform()->pos(), cam.frustum(),
                      mesh_, node_dimension_);
    mesh_.render();
  }

  // render with uniforms
  void render(const engine::Camera& cam,
              const gl::UniformObject<glm::vec4>& uRenderData) {
    mesh_.clearRenderList();
    root_.selectNodes(cam.transform()->pos(), cam.frustum(),
                      mesh_, node_dimension_);
    mesh_.render(uRenderData);
  }
};

}  // namespace cdlod
}  // namespace engine

#endif

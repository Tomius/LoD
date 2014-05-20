// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include "grid_mesh_renderer.h"
#include "../collision/bounding_box.h"
#include "../../skybox.h" // debug
#include "../../shadow.h" // debug

namespace engine {

class CDLODQuadTree : public engine::GameObject {
  GridMeshRenderer renderer_;
  GLubyte node_dimension_;

  struct Node {
    GLshort x, z;
    GLshort min_y, max_y;
    GLushort size;
    GLubyte level;
    std::unique_ptr<Node> tl, tr, bl, br;

    Node(GLshort x, GLshort z, GLubyte level, GLubyte dimension)
        : x(x)
        , z(z)
        , size(dimension * (1 << level))
        , level(level)
        , tl(nullptr)
        , tr(nullptr)
        , bl(nullptr)
        , br(nullptr) {
      if (level > 0) {
        tl = std::unique_ptr<Node>(new Node(x-size/4, z+size/4, level-1, dimension));
        tr = std::unique_ptr<Node>(new Node(x+size/4, z+size/4, level-1, dimension));
        bl = std::unique_ptr<Node>(new Node(x-size/4, z-size/4, level-1, dimension));
        br = std::unique_ptr<Node>(new Node(x+size/4, z-size/4, level-1, dimension));
      }
    }

    BoundingBox boundingBox() const {
      return BoundingBox(glm::vec3(x-size/2, min_y, z-size/2),
                         glm::vec3(x+size/2, max_y, z+size/2));
    }

    bool collidesWithSphere(const glm::vec3& center, float radius) {
      return boundingBox().collidesWithSphere(center, radius);
    }

    svec2 getMinMaxOfArea(const GridMeshRenderer& renderer) {
      if (level == 0) {
        renderer.getMinMaxOfArea(x, z, size, &min_y, &max_y);
        return svec2{min_y, max_y};
      } else {
        svec2 tlb = tl->getMinMaxOfArea(renderer);
        svec2 trb = tr->getMinMaxOfArea(renderer);
        svec2 blb = bl->getMinMaxOfArea(renderer);
        svec2 brb = br->getMinMaxOfArea(renderer);
        min_y = std::min(tlb.x, std::min(trb.x, std::min(blb.x, brb.x)));
        max_y = std::max(tlb.y, std::max(trb.y, std::max(blb.y, brb.y)));
        return svec2{min_y, max_y};
      }
    }

    void traverse(const glm::vec3& cam_pos, const Frustum& frustum,
                  GridMeshRenderer& renderer, int node_dimension) {
      float scale = 1 << level;
      float lod_range = scale * 128;
      BoundingBox bb = boundingBox();

      if(!bb.collidesWithFrustum(frustum)) {
        return;
      }

      // if we can cover the whole area or if we are a leaf
      if (!bb.collidesWithSphere(cam_pos, lod_range) || level == 0) {
        renderer.addToRenderList(glm::vec2(x, z), scale, level);
      } else {
        bool btl = tl->collidesWithSphere(cam_pos, lod_range);
        bool btr = tr->collidesWithSphere(cam_pos, lod_range);
        bool bbl = bl->collidesWithSphere(cam_pos, lod_range);
        bool bbr = br->collidesWithSphere(cam_pos, lod_range);

        // Ask childs to render what we can't
        if (btl) {
          tl->traverse(cam_pos, frustum, renderer, node_dimension);
        }
        if (btr) {
          tr->traverse(cam_pos, frustum, renderer, node_dimension);
        }
        if (bbl) {
          bl->traverse(cam_pos, frustum, renderer, node_dimension);
        }
        if (bbr) {
          br->traverse(cam_pos, frustum, renderer, node_dimension);
        }

        // Render, what the childs didn't do
        renderer.addToRenderList(glm::vec2(x, z), scale, level,
                                 !btl, !btr, !bbl, !bbr);
      }
    }
  };

  std::unique_ptr<Node> root_;

 public:
  CDLODQuadTree(Skybox* skybox, Shadow *shadow, int node_dimension = 32)
      : renderer_(skybox, shadow, node_dimension)
      , node_dimension_(node_dimension)
      , root_(new Node(0, 0, 14 - log2(node_dimension), node_dimension)) {
    std::cout << "node dimension: " << (int)node_dimension << std::endl;
    root_->getMinMaxOfArea(renderer_);
  }

  GLubyte node_dimension() const {
    return node_dimension_;
  }

  void set_node_dimension(GLubyte node_dimension) {
    std::cout << "node dimension: " << (int)node_dimension << std::endl;
    node_dimension_ = node_dimension;
    renderer_.set_dimension(node_dimension);
    root_ = std::unique_ptr<Node>{new Node(0, 0, 14 - log2(node_dimension), node_dimension)};
    root_->getMinMaxOfArea(renderer_);
  }

  void inc_node_dimension() {
    if(node_dimension_ < 128) {
      set_node_dimension(node_dimension_*2);
    }
  }

  void dec_node_dimension() {
    if(node_dimension_ > 1) {
      set_node_dimension(node_dimension_/2);
    }
  }

  virtual void render(float time, const engine::Camera& cam) override {
    renderer_.clearRenderList();
    renderer_.setupRender(cam);
    root_->traverse(cam.pos(), cam.frustum(), renderer_, node_dimension_);
    renderer_.render();
  }

  double getHeight(double x, double y) const {
    return renderer_.getHeight(x, y);
  }

  int w() const {
    return renderer_.w();
  }

  int h() const {
    return renderer_.h();
  }

};

} // namespace engine

#endif

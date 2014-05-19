// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include "grid_mesh_renderer.h"
#include "../collision/bounding_box.h"

namespace engine {

class CDLODQuadTree : public engine::GameObject {
  GridMeshRenderer renderer_;

  struct Node {
    GLshort x, z;
    GLshort min_y, max_y;
    GLushort level;
    GLushort size;
    std::unique_ptr<Node> tl, tr, bl, br;

    Node(GLshort x, GLshort z, GLushort level)
        : x(x)
        , z(z)
        , min_y(0)
        , max_y(0)
        , level(level)
        , size(GridMeshRenderer::max_dim * (1 << level))
        , tl(nullptr)
        , tr(nullptr)
        , bl(nullptr)
        , br(nullptr) {

      if (level > 0) {
        tl = std::unique_ptr<Node>(new Node(x-size/4, z+size/4, level-1));
        tr = std::unique_ptr<Node>(new Node(x+size/4, z+size/4, level-1));
        bl = std::unique_ptr<Node>(new Node(x-size/4, z-size/4, level-1));
        br = std::unique_ptr<Node>(new Node(x+size/4, z-size/4, level-1));
      }
    }

    BoundingBox boundingBox() const {
      return BoundingBox(glm::vec3(x-size/2, min_y, z-size/2),
                         glm::vec3(x+size/2, max_y, z+size/2));
    }

    bool collidesWithSphere(const glm::vec3& center, float radius) {
      return boundingBox().collidesWithSphere(center, radius);
    }


    void traverse(const glm::vec3& cam_pos, const Frustum& frustum,
                GridMeshRenderer& renderer) {
      float scale = float(size) / GridMeshRenderer::max_dim;
      BoundingBox bb = boundingBox();

      if(!bb.collidesWithFrustum(frustum)) {
        return;
      }

      // if we can cover the whole area or if we are a leaf
      if (!bb.collidesWithSphere(cam_pos, size) || level == 0) {
        renderer.addToRenderList(glm::vec2(x, z), scale, level);
      } else {
        bool btl = tl->collidesWithSphere(cam_pos, size);
        bool btr = tr->collidesWithSphere(cam_pos, size);
        bool bbl = bl->collidesWithSphere(cam_pos, size);
        bool bbr = br->collidesWithSphere(cam_pos, size);

        // Ask childs to render what we can't
        if (btl) {
          tl->traverse(cam_pos, frustum, renderer);
        }
        if (btr) {
          tr->traverse(cam_pos, frustum, renderer);
        }
        if (bbl) {
          bl->traverse(cam_pos, frustum, renderer);
        }
        if (bbr) {
          br->traverse(cam_pos, frustum, renderer);
        }

        // Render, what the childs didn't do
        renderer.addToRenderList(glm::vec2(x, z), scale, level, !btl, !btr, !bbl, !bbr);
      }
    }
  } root_;

 public:
  CDLODQuadTree() : renderer_(32), root_(0, 0, 5) { }

  virtual void render(float time, const engine::Camera& cam) override {
    renderer_.clearRenderList();
    root_.traverse(cam.pos(), cam.frustum(), renderer_);
    renderer_.render(cam);
  }

};

} // namespace engine

#endif

// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include "grid_mesh_renderer.h"
#include "../collision/bounding_box.h"
#include "../../skybox.h" // debug

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
        , level(level)
        , size(GridMeshRenderer::node_dim * (1 << level))
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
        max_y = std::min(tlb.y, std::min(trb.y, std::min(blb.y, brb.y)));
        return svec2{min_y, max_y};
      }
    }

    void traverse(const glm::vec3& cam_pos, const Frustum& frustum,
                GridMeshRenderer& renderer) {
      float scale = float(size) / GridMeshRenderer::node_dim;
      BoundingBox bb = boundingBox();

      // if(!bb.collidesWithFrustum(frustum)) {
      //   return;
      // }

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
  CDLODQuadTree(Skybox* skybox)
      : renderer_(skybox, 128)
      , root_(0, 0, 7) {
    root_.getMinMaxOfArea(renderer_);
  }

  virtual void render(float time, const engine::Camera& cam) override {
    renderer_.clearRenderList();
    renderer_.setup_render(cam);
    root_.traverse(cam.pos(), cam.frustum(), renderer_);
    renderer_.render();
  }

};

} // namespace engine

#endif

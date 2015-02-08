// Copyright (c) 2014, Tamas Csala

#include <thread>
#include <algorithm>
#include "./quad_tree.h"
#include "../misc.h"

namespace engine {
namespace cdlod {

QuadTree::Node::Node(GLshort x, GLshort z, GLubyte level,
                     GLubyte dimension, bool root)
    : x(x), z(z), size(dimension * (1 << level)), level(level)
    , tl(nullptr), tr(nullptr), bl(nullptr), br(nullptr) {
  if (level > 0) {
    if (root) {
      // The creation of say a 14-depth quadtree is slow. Better run it in
      // four threads
      std::thread th_tl{Init, x-size/4, z+size/4, level-1, dimension, &tl};
      std::thread th_tr{Init, x+size/4, z+size/4, level-1, dimension, &tr};
      std::thread th_bl{Init, x-size/4, z-size/4, level-1, dimension, &bl};
      std::thread th_br{Init, x+size/4, z-size/4, level-1, dimension, &br};
      th_tl.join(); th_tr.join(); th_bl.join(); th_br.join();
    } else {
      tl = std::unique_ptr<Node>(new Node(x-size/4, z+size/4, level-1, dimension));
      tr = std::unique_ptr<Node>(new Node(x+size/4, z+size/4, level-1, dimension));
      bl = std::unique_ptr<Node>(new Node(x-size/4, z-size/4, level-1, dimension));
      br = std::unique_ptr<Node>(new Node(x+size/4, z-size/4, level-1, dimension));
    }
  }
}

void QuadTree::Node::Init(GLshort x, GLshort z, GLubyte level,
                          GLubyte dimension, std::unique_ptr<Node>* node) {
  *node = make_unique<Node>(x, z, level, dimension);
}

void QuadTree::Node::countMinMaxOfArea(const HeightMapInterface& hmap,
                                       double *min, double *max, bool root) {
  glm::dvec2 min_xz(x-size/2, z-size/2);
  glm::dvec2 max_xz(x+size/2, z+size/2);

  if (level == 0) {
    glm::dvec2 min_max_y = hmap.getMinMaxOfArea(x, z, size, size);
    *min = min_max_y.x;
    *max = min_max_y.y;
  } else {
    double tl_min, tr_min, bl_min, br_min;
    double tl_max, tr_max, bl_max, br_max;
    if (root) {
      std::thread th_tl{CountMinMaxOfArea, tl.get(), std::ref(hmap), &tl_min, &tl_max};
      std::thread th_tr{CountMinMaxOfArea, tr.get(), std::ref(hmap), &tr_min, &tr_max};
      std::thread th_bl{CountMinMaxOfArea, bl.get(), std::ref(hmap), &bl_min, &bl_max};
      std::thread th_br{CountMinMaxOfArea, br.get(), std::ref(hmap), &br_min, &br_max};
      th_tl.join(); th_tr.join(); th_bl.join(); th_br.join();
    } else {
      tl->countMinMaxOfArea(hmap, &tl_min, &tl_max);
      tr->countMinMaxOfArea(hmap, &tr_min, &tr_max);
      bl->countMinMaxOfArea(hmap, &bl_min, &bl_max);
      br->countMinMaxOfArea(hmap, &br_min, &br_max);
    }
    *min = std::min(tl_min, std::min(tr_min, std::min(bl_min, br_min)));
    *max = std::max(tl_max, std::max(tr_max, std::max(bl_max, br_max)));
  }

  bbox = BoundingBox{glm::vec3(min_xz.x, *min, min_xz.y),
                     glm::vec3(max_xz.x, *max, max_xz.y)};
}

void QuadTree::Node::selectNodes(const glm::vec3& cam_pos,
                                 const Frustum& frustum,
                                 QuadGridMesh& grid_mesh,
                                 int node_dimension) {
  float scale = 1 << level;
  float lod_range = scale * 128;

  if (!bbox.collidesWithFrustum(frustum)) { return; }

  // if we can cover the whole area or if we are a leaf
  if (!bbox.collidesWithSphere(cam_pos, lod_range) || level == 0) {
    grid_mesh.addToRenderList(x, z, scale, level);
  } else {
    bool btl = tl->collidesWithSphere(cam_pos, lod_range);
    bool btr = tr->collidesWithSphere(cam_pos, lod_range);
    bool bbl = bl->collidesWithSphere(cam_pos, lod_range);
    bool bbr = br->collidesWithSphere(cam_pos, lod_range);

    // Ask childs to render what we can't
    if (btl) {
      tl->selectNodes(cam_pos, frustum, grid_mesh, node_dimension);
    }
    if (btr) {
      tr->selectNodes(cam_pos, frustum, grid_mesh, node_dimension);
    }
    if (bbl) {
      bl->selectNodes(cam_pos, frustum, grid_mesh, node_dimension);
    }
    if (bbr) {
      br->selectNodes(cam_pos, frustum, grid_mesh, node_dimension);
    }

    // Render, what the childs didn't do
    grid_mesh.addToRenderList(x, z, scale, level, !btl, !btr, !bbl, !bbr);
  }
}

}  // namespace cdlod
}  // namespace engine

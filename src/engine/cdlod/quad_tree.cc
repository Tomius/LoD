#include "quad_tree.h"

namespace engine {

namespace cdlod {

QuadTree::Node::Node(GLshort x, GLshort z, GLubyte level, GLubyte dimension)
    : x(x), z(z), size(dimension * (1 << level)), level(level)
    , tl(nullptr), tr(nullptr), bl(nullptr), br(nullptr) {
  if (level > 0) {
    tl = std::unique_ptr<Node>(new Node(x-size/4, z+size/4, level-1, dimension));
    tr = std::unique_ptr<Node>(new Node(x+size/4, z+size/4, level-1, dimension));
    bl = std::unique_ptr<Node>(new Node(x-size/4, z-size/4, level-1, dimension));
    br = std::unique_ptr<Node>(new Node(x+size/4, z-size/4, level-1, dimension));
  }
}

glm::dvec2 QuadTree::Node::countMinMaxOfArea(const HeightMapInterface& hmap) {
  glm::dvec2 min_max_y;
  glm::dvec2 min_xz = hmap.toWorldSpace(x-size/2, z-size/2);
  glm::dvec2 max_xz = hmap.toWorldSpace(x+size/2, z+size/2);

  if (level == 0) {
    min_max_y = hmap.getMinMaxOfArea(x, z, size, size);
  } else {
    glm::dvec2 tlb = tl->countMinMaxOfArea(hmap);
    glm::dvec2 trb = tr->countMinMaxOfArea(hmap);
    glm::dvec2 blb = bl->countMinMaxOfArea(hmap);
    glm::dvec2 brb = br->countMinMaxOfArea(hmap);
    min_max_y.x = std::min(tlb.x, std::min(trb.x, std::min(blb.x, brb.x)));
    min_max_y.y = std::max(tlb.y, std::max(trb.y, std::max(blb.y, brb.y)));
  }

  bbox = BoundingBox{glm::vec3(min_xz.x, min_max_y.x, min_xz.y),
                     glm::vec3(max_xz.x, min_max_y.y, max_xz.y)};

  return min_max_y;
}

void QuadTree::Node::selectNodes(const glm::vec3& cam_pos,
                                 const Frustum& frustum,
                                 QuadGridMesh& grid_mesh,
                                 int node_dimension) {
  float scale = 1 << level;
  float lod_range = scale * 128;

  if(!bbox.collidesWithFrustum(frustum)) {
    return;
  }

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

} // namespace cdlod

} // namespace engine

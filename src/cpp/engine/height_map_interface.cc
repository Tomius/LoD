#include "height_map_interface.h"

namespace engine {

glm::dvec2 HeightMapInterface::getMinMaxOfArea(int x, int y, int w, int h) const {
  double zero = 0.0;
  double infinity = 1.0 / zero;
  double curr_min = infinity, curr_max = -infinity;

  for (int i = x - w/2; i <= x + w/2; ++i) {
    for (int j = y - h/2; j <= y + h/2; ++j) {
      if (valid(i, j)) {
        int curr_height = heightAt(i, j);
        if(curr_height < curr_min) {
          curr_min = curr_height;
        }
        if(curr_height > curr_max) {
          curr_max = curr_height;
        }
      }
    }
  }

  if(isinf(curr_min)) {
    curr_min = 0;
  }
  if(isinf(curr_max)) {
    curr_max = 0;
  }

  return glm::dvec2(curr_min, curr_max);
}

}

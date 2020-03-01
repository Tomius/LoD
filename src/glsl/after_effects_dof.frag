#version 130

#include "sky.frag"
#include "depth_of_field.frag"
#include "lens_flare.frag"
#include "after_effects.frag"

void main() {
  FetchNeighbours();
  vec3 color = Glow() + DoF(CurrentPixel());
  color = FilmicToneMap(color);
  if (SunPos().y > 0) {
    color += LensFlare();
  }
  gl_FragColor = vec4(clamp(color, vec3(0), vec3(1)), 1);
}

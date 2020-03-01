// Copyright (c) 2014, Tamas Csala

#version 430

#include "sky.frag"
#include "lens_flare.frag"

#export void FetchNeighbours();
#export vec3 Glow();
#export vec3 FilmicToneMap(vec3 color);
#export vec3 CurrentPixel();

uniform sampler2D uTex;
uniform vec2 uScreenSize;

ivec2 tex_coord = ivec2(gl_FragCoord.xy);

/* 0 1 2
   3 4 5
   6 7 8 */

vec3 neighbours[9];

vec3 Fetch(ivec2 coord) {
  return texture2D(uTex, coord/uScreenSize).rgb;
}

void FetchNeighbours() {
  neighbours[0] = Fetch(tex_coord + ivec2(-1, -1));
  neighbours[1] = Fetch(tex_coord + ivec2(-1,  0));
  neighbours[2] = Fetch(tex_coord + ivec2(-1, +1));
  neighbours[3] = Fetch(tex_coord + ivec2( 0, -1));
  neighbours[4] = Fetch(tex_coord);
  neighbours[5] = Fetch(tex_coord + ivec2( 0, +1));
  neighbours[6] = Fetch(tex_coord + ivec2(+1, -1));
  neighbours[7] = Fetch(tex_coord + ivec2(+1,  0));
  neighbours[8] = Fetch(tex_coord + ivec2(+1, +1));
}

// Faster than pow(v, 2)
vec3 sqr(vec3 v) { return v*v; }

// This should be two nested for cycles if performance wasn't an issue...
vec3 Glow() {
  vec3 sum = sqr(neighbours[0])
           + sqr(neighbours[1]) * 2
           + sqr(neighbours[2])
           + sqr(neighbours[3]) * 2
           + sqr(neighbours[4]) * 4
           + sqr(neighbours[5]) * 2
           + sqr(neighbours[6])
           + sqr(neighbours[7]) * 2
           + sqr(neighbours[8]);

  return sum / 8;
}

vec3 FilmicToneMap(vec3 color) {
  vec3 x = max(color - 0.004, 0);
  return (x*(6.2*x+0.5)) / (x*(6.2*x+1.7)+0.06);
}

vec3 CurrentPixel() {
  return neighbours[4];
}

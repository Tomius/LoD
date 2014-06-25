// Copyright (c) 2014, Tamas Csala

#version 120

uniform sampler2D uTex, uDepthTex;
uniform float zNear = 1.0, zFar = 3000.0;
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
float sqr(float v) { return v*v; }

// This should be two nested for cycles if performance wasn't an issue...
vec3 Glow() {
  return (sqr(neighbours[0])
        + sqr(neighbours[1]) * 2
        + sqr(neighbours[2])
        + sqr(neighbours[3]) * 2
        + sqr(neighbours[4]) * 4
        + sqr(neighbours[5]) * 2
        + sqr(neighbours[6])
        + sqr(neighbours[7]) * 2
        + sqr(neighbours[8])) / 4;
}

// see http://web.archive.org/web/20130416194336/http://olivers.posterous.com/linear-depth-in-glsl-for-real
float DistanceFromCamera() {
  float z_b = texture2D(uDepthTex, tex_coord/uScreenSize).x;
  float z_n = 2.0 * z_b - 1.0;
  return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}

vec3 DoF() {
  vec3 blurred = texture2DLod(uTex, tex_coord/uScreenSize, 3).rgb;
  float depth_alpha = DistanceFromCamera() / zFar;
  return mix(neighbours[4], blurred, depth_alpha);
}

vec3 FilmicToneMap(vec3 color) {
  vec3 x = max(color - 0.004, 0);
  return (x*(6.2*x+0.5)) / (x*(6.2*x+1.7)+0.06);
}

void main() {
  FetchNeighbours();
  vec3 color = Glow() + DoF();
  gl_FragColor = vec4(FilmicToneMap(color), 1.0);
}

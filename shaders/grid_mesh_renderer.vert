// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 aPosition;
uniform mat4 uProjectionMatrix, uCameraMatrix;
uniform vec2 uOffset = vec2(0);
uniform float uScale = 1;
uniform vec3 uCamPos;
uniform int uLevel;

const float max_node_dim = 128;

vec2 frac(vec2 x) { return x - floor(x); }

vec2 morphVertex(vec2 vertex, float morphK ) {
  vec2 fracPart = frac(vertex/ uScale * 0.5 ) * 2.0;
  return vertex - fracPart * uScale * morphK;
}


void main() {
  vec2 pos = uOffset + uScale * aPosition;

  float max_dist = pow(2, uLevel+1) * max_node_dim;
  float dist = length(uCamPos - vec3(pos.x, 0, pos.y));
  float morph = clamp((dist - 0.667*max_dist) / (0.333 * max_dist), 0, 1);
  pos = morphVertex(pos, morph);

  gl_Position = uProjectionMatrix * (uCameraMatrix *
                vec4(vec3(pos.x, 0, pos.y), 1));
}

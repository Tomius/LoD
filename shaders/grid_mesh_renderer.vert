// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 aPosition;
attribute vec4 aRenderData;

// Vertex attrib divisor works like a uniform
vec2 uOffset = aRenderData.xy;
float uScale = aRenderData.z;
int uLevel = int(aRenderData.w);

uniform mat4 uProjectionMatrix, uCameraMatrix;
uniform vec3 uCamPos;

const float max_node_dim = 128;

vec2 frac(vec2 x) { return x - floor(x); }

vec2 morphVertex(vec2 vertex, float morphK ) {
  vec2 fracPart = frac(vertex/ uScale * 0.5 ) * 2.0;
  return vertex - fracPart * uScale * morphK;
}

const float morph_start = 0.8333;

void main() {
  vec2 pos = uOffset + uScale * aPosition;


  float max_dist = pow(2, uLevel+1) * max_node_dim;
  float dist = length(uCamPos - vec3(pos.x, 0, pos.y));

  float morph =
    clamp((dist - morph_start*max_dist) / ((1-morph_start) * max_dist), 0, 1);

  pos = morphVertex(pos, morph);

  gl_Position = uProjectionMatrix * (uCameraMatrix *
                vec4(vec3(pos.x, 0, pos.y), 1));
}

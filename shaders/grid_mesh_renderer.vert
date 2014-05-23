// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 aPosition;

#define VERTEX_ATTRIB_DIVISOR true

#ifdef VERTEX_ATTRIB_DIVISOR
  attribute vec4 uRenderData; // Vertex attrib divisor works like a uniform
#else
  uniform vec4 uRenderData;
#endif

vec2 uOffset = uRenderData.xy;
float uScale = uRenderData.z;
int uLevel = int(uRenderData.w);

uniform mat4 uProjectionMatrix, uCameraMatrix;
uniform vec3 uCamPos;

uniform sampler2D uHeightMap;
uniform vec2 uTexSize;

varying vec3  w_vNormal;
varying vec3  c_vPos, w_vPos;
varying vec2  vTexcoord;
varying float vInvalid;
varying mat3  vNormalMatrix;

float fetchHeight(vec2 texCoord) {
  return texture2D(uHeightMap, texCoord).r * 255;
}

vec2 frac(vec2 x) { return x - floor(x); }

vec2 morphVertex(vec2 vertex, float morphK ) {
  vec2 fracPart = frac(vertex/ uScale * 0.5 ) * 2.0;
  return vertex - fracPart * uScale * morphK;
}

const float morph_start = 0.90;
const float morph_end_fudge = 0.99;

void main() {
  vec2 pos = uOffset + uScale * aPosition;

  float max_dist = morph_end_fudge * pow(2, uLevel+1) * 128;
  float dist = length(uCamPos - vec3(pos.x, 0, pos.y));

  float morph =
    clamp((dist - morph_start*max_dist) / ((1-morph_start) * max_dist), 0, 1);

  vec2 morphed_pos = morphVertex(pos, morph);
  vec2 texcoord = morphed_pos + uTexSize/2;

  if (texcoord.x < 1 || uTexSize.x < texcoord.x + 1 ||
      texcoord.y < 1 || uTexSize.y < texcoord.y + 1) {
    vInvalid = 1e10;
    gl_Position = vec4(0.0);
    return;
  } else {
    vInvalid = 0.0;
  }

  texcoord /= vec2(uTexSize);

  vTexcoord = texcoord;

  float height = fetchHeight(texcoord);
  vec3 w_pos = vec3(morphed_pos.x, height, morphed_pos.y);
  w_vPos = w_pos;

  vec4 c_pos = uCameraMatrix * vec4(w_pos, 1);
  c_vPos = c_pos.xyz;

  // Normal approximation from the heightmap
  vec3 u = vec3(1.0f, 0.0f, fetchHeight(texcoord + vec2(1, 0)) -
                            fetchHeight(texcoord - vec2(1, 0)));
  vec3 v = vec3(0.0f, 1.0f, fetchHeight(texcoord + vec2(0, 1)) -
                            fetchHeight(texcoord + vec2(0, 1)));
  vec3 w_normal = normalize(cross(u, v));
  w_vNormal = w_normal;

  vec3 w_tangent = cross(vec3(0.0, 0.0, 1.0), w_normal);
  vec3 w_bitangent = cross(w_normal, w_tangent);

  vNormalMatrix[0] = w_tangent;
  vNormalMatrix[1] = w_bitangent;
  vNormalMatrix[2] = w_normal;

  gl_Position = uProjectionMatrix * c_pos;
}

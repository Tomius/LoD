// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 CDLODTerrain_aPosition; // I hate the lack of namespaces

#define VERTEX_ATTRIB_DIVISOR true

#ifdef VERTEX_ATTRIB_DIVISOR
  // Vertex attrib divisor works like a uniform
  attribute vec4 CDLODTerrain_uRenderData;
#else
  uniform vec4 CDLODTerrain_uRenderData;
#endif

vec2 CDLODTerrain_uOffset = CDLODTerrain_uRenderData.xy;
float CDLODTerrain_uScale = CDLODTerrain_uRenderData.z;
int CDLODTerrain_uLevel = int(CDLODTerrain_uRenderData.w);

uniform sampler2D CDLODTerrain_uHeightMap;
uniform vec2 CDLODTerrain_uTexSize;
uniform vec3 CDLODTerrain_uCamPos;

float CDLODTerrain_fetchHeight(vec2 tex_coord) {
  return texture2D(CDLODTerrain_uHeightMap,
                   tex_coord / vec2(CDLODTerrain_uTexSize)).r * 255;
}

vec2 CDLODTerrain_frac(vec2 x) { return x - floor(x); }

vec2 CDLODTerrain_morphVertex(vec2 vertex, float morph) {
  vec2 frac_part = CDLODTerrain_frac(vertex / CDLODTerrain_uScale * 0.5 ) * 2.0;
  return vertex - frac_part * CDLODTerrain_uScale * morph;
}

const float CDLODTerrain_morph_start = 0.85;
const float CDLODTerrain_morph_end_fudge = 0.99;

vec3 CDLODTerrain_worldPos() {
  vec2 pos = CDLODTerrain_uOffset + CDLODTerrain_uScale * CDLODTerrain_aPosition;

  float max_dist = CDLODTerrain_morph_end_fudge * pow(2, CDLODTerrain_uLevel+1) * 128;
  float dist = length(CDLODTerrain_uCamPos - vec3(pos.x, CDLODTerrain_fetchHeight(pos), pos.y));

  float morph = clamp((dist - CDLODTerrain_morph_start*max_dist) /
      ((1-CDLODTerrain_morph_start) * max_dist), 0, 1);

  vec2 morphed_pos = CDLODTerrain_morphVertex(pos, morph);

  return vec3(morphed_pos.x, CDLODTerrain_fetchHeight(morphed_pos), morphed_pos.y);
}

vec2 CDLODTerrain_texCoord(vec3 pos) {
  return pos.xz / CDLODTerrain_uTexSize;
}

vec3 CDLODTerrain_normal(vec3 pos) {
  vec3 u = vec3(1.0f, CDLODTerrain_fetchHeight(pos.xz + vec2(1, 0)) -
                      CDLODTerrain_fetchHeight(pos.xz - vec2(1, 0)), 0.0f);
  vec3 v = vec3(0.0f, CDLODTerrain_fetchHeight(pos.xz + vec2(0, 1)) -
                      CDLODTerrain_fetchHeight(pos.xz - vec2(0, 1)), 1.0f);
  return normalize(cross(u, -v));
}

mat3 CDLODTerrain_normalMatrix(vec3 normal) {
  vec3 tangent = normalize(cross(normal, vec3(0.0, 0.0, 1.0)));
  vec3 bitangent = normalize(cross(normal, tangent));
  tangent = normalize(cross(bitangent, normal));

  return mat3(tangent, bitangent, normal);
}

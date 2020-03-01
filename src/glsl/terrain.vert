// Copyright (c) 2014, Tamas Csala

#version 430

#include "engine/cdlod_terrain.vert"

uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;
uniform vec2 CDLODTerrain_uTexSize;

out vec3  w_vNormal;
out vec3  c_vPos, w_vPos;
out vec2  vTexCoord;
out float vInvalid;
out mat3  vNormalMatrix;

void main() {
  vec3 w_pos = CDLODTerrain_worldPos();
  vec2 tex_coord = CDLODTerrain_texCoord(w_pos);
  vec3 offseted_w_pos = (uModelMatrix * vec4(w_pos, 1)).xyz;

  vec2 bias = vec2(1) / CDLODTerrain_uTexSize;
  if (tex_coord.x < 0 + bias.x || 1 - bias.x < tex_coord.x ||
      tex_coord.y < 0 + bias.y || 1 - bias.y < tex_coord.y) {
    vInvalid = 1e10;
    gl_Position = vec4(0.0);
    return;
  } else {
    vInvalid = 0.0;
  }

  w_vPos = offseted_w_pos;
  vTexCoord = tex_coord;

  vec4 c_pos = uCameraMatrix * vec4(offseted_w_pos, 1);
  c_vPos = vec3(c_pos);

  vec3 w_normal = CDLODTerrain_normal(w_pos);
  w_vNormal = w_normal;

  vNormalMatrix = CDLODTerrain_normalMatrix(w_normal);

  gl_Position = uProjectionMatrix * c_pos;
}

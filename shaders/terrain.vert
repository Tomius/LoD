// Copyright (c) 2014, Tamas Csala

#version 120

uniform mat4 uProjectionMatrix, uCameraMatrix;

varying vec3  w_vNormal;
varying vec3  c_vPos, w_vPos;
varying vec2  vTexCoord;
varying float vInvalid;
varying mat3  vNormalMatrix;

vec3 CDLODTerrain_worldPos();
vec2 CDLODTerrain_texCoord(vec3 pos);
vec3 CDLODTerrain_normal(vec3 pos);
mat3 CDLODTerrain_normalMatrix(vec3 normal);

void main() {
  vec3 w_pos = CDLODTerrain_worldPos();
  vec2 tex_coord = CDLODTerrain_texCoord(w_pos);

  if (tex_coord.x <= 0 || 1 <= tex_coord.x ||
      tex_coord.y <= 0 || 1 <= tex_coord.y) {
    vInvalid = 1e10;
    gl_Position = vec4(0.0);
    return;
  } else {
    vInvalid = 0.0;
  }

  w_vPos = w_pos;
  vTexCoord = tex_coord;

  vec4 c_pos = uCameraMatrix * vec4(w_pos, 1);
  c_vPos = vec3(c_pos);

  vec3 w_normal = CDLODTerrain_normal(w_pos);
  w_vNormal = w_normal;

  vNormalMatrix = CDLODTerrain_normalMatrix(w_normal);

  gl_Position = uProjectionMatrix * c_pos;
}

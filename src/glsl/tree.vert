// Copyright (c) 2014, Tamas Csala

#version 430

in vec4 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;

uniform mat4 uModelCameraMatrix, uProjectionMatrix;
uniform mat3 uNormalMatrix;

out vec3 c_vPos;
out vec3 w_vNormal;
out vec2 vTexCoord;

void main() {
  w_vNormal = aNormal * uNormalMatrix;
  vTexCoord = aTexCoord;

  vec4 c_pos = uModelCameraMatrix * aPosition;
  c_vPos = vec3(c_pos);

  gl_Position = uProjectionMatrix * c_pos;
}

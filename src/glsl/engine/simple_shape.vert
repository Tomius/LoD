// Copyright (c) 2014, Tamas Csala

#version 430

in vec4 aPosition;
in vec3 aNormal;

uniform mat4 uProjectionMatrix = mat4(1.0),
             uCameraMatrix = mat4(1.0),
             uModelMatrix = mat4(1.0);

out vec3 w_vNormal;

void main() {
  w_vNormal = mat3(uModelMatrix) * aNormal;
  gl_Position = uProjectionMatrix * (uCameraMatrix * (uModelMatrix*aPosition));
}

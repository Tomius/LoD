// Copyright (c) 2014, Tamas Csala

#version 130

attribute vec3 aPosition;

uniform mat4 uProjectionMatrix;
uniform mat3 uCameraMatrix;

varying vec3 vTexCoord;

void main(void) {
  gl_Position = uProjectionMatrix * vec4(uCameraMatrix * vec3(10 * aPosition), 1.0);
  vTexCoord = aPosition;
}

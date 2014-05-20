// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec3 aPosition;
uniform mat4 uProjectionMatrix, uCameraMatrix;
uniform vec2 uOffset, uMinMax;
uniform float uScale = 1;
uniform int uLevel;


void main() {
  vec3 pos = vec3(uOffset.x, 0, uOffset.y) + 0.9 * uScale * aPosition;
  if(pos.y > 0) {
    pos.y = uMinMax.y - 1;
  } else {
    pos.y = uMinMax.x + 1;
  }
  gl_Position = uProjectionMatrix * (uCameraMatrix * vec4(pos, 1));
}

// Copyright (c) 2014, Tamas Csala

attribute vec4 aPosition;
attribute vec2 aTexcoord;

uniform mat4 uProjectionMatrix;
uniform vec2 uOffset;

varying vec2 vTexcoord;

void main() {
  vTexcoord = aTexcoord;
  gl_Position = uProjectionMatrix*aPosition + vec4(uOffset, 0, 0);
}

// Copyright (c) 2014, Tamas Csala

attribute vec4 aPosition;
attribute vec2 aTexCoord;

uniform mat4 uProjectionMatrix;
uniform vec2 uOffset;

varying vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoord;
  gl_Position = uProjectionMatrix*aPosition + vec4(uOffset, 0, 0);
}

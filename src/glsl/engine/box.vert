// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 aPosition, aTexCoord;

uniform vec2 uOffset, uScale;

varying vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoord;
  gl_Position = vec4(uScale * aPosition + uOffset, 0, 1);
}

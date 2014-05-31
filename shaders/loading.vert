// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 aPosition;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoord;
  gl_Position = vec4(aPosition, 0, 1);
}

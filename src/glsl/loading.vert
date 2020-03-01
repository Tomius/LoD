// Copyright (c) 2014, Tamas Csala

#version 130

attribute vec4 aPosition;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

void main() {
  vTexCoord = vec2(aTexCoord.s, 1-aTexCoord.t);
  gl_Position = aPosition;
}

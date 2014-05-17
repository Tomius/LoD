// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 aPosition;
attribute vec2 aTexcoord;

varying vec2 vTexcoord;

void main() {
  vTexcoord = aTexcoord;
  gl_Position = vec4(aPosition, 0, 1);
}

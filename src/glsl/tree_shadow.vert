// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec4 aPosition;
attribute vec2 aTexCoord;

uniform mat4 uMCP;

varying vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoord;
  gl_Position = uMCP * aPosition;
}

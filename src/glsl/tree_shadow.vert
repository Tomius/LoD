// Copyright (c) 2014, Tamas Csala

#version 430

in vec4 aPosition;
in vec2 aTexCoord;

uniform mat4 uMCP;

out vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoord;
  gl_Position = uMCP * aPosition;
}

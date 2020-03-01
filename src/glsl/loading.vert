// Copyright (c) 2014, Tamas Csala

#version 430

in vec4 aPosition;
in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
  vTexCoord = vec2(aTexCoord.s, 1-aTexCoord.t);
  gl_Position = aPosition;
}

// Copyright (c) 2014, Tamas Csala

#version 430

in vec2 aPosition, aTexCoord;

uniform vec2 uOffset, uScale;

out vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoord;
  gl_Position = vec4(uScale * aPosition + uOffset, 0, 1);
}

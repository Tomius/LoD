// Copyright (c) 2014, Tamas Csala

#version 430

in vec2 vTexCoord;

uniform sampler2D uDiffuseTexture;

void main() {
  if (texture2D(uDiffuseTexture, vTexCoord).a < 1.0)
    discard;
}

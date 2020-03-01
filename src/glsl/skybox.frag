// Copyright (c) 2014, Tamas Csala

#version 430

#include "sky.frag"

in vec3 vTexCoord;

out vec4 fragColor;

void main() {
  fragColor = vec4(SkyColor(normalize(vTexCoord)), 1.0);
}

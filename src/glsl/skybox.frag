// Copyright (c) 2014, Tamas Csala

#version 130

#include "sky.frag"

varying vec3 vTexCoord;

void main() {
  gl_FragColor = vec4(SkyColor(normalize(vTexCoord)), 1.0);
}

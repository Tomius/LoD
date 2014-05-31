// Copyright (c) 2014, Tamas Csala

#version 120

varying vec3 vTexCoord;

vec3 SkyColor(vec3 lookDir);

void main() {
  gl_FragColor = vec4(SkyColor(normalize(vTexCoord)), 1.0);
}

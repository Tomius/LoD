// Copyright (c) 2014, Tamas Csala

#version 130

varying vec2 vTexCoord;

uniform sampler2D uTex;

void main() {
  gl_FragColor = vec4(texture2D(uTex, vTexCoord).rgb, 1.0);
}

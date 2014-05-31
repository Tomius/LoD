// Copyright (c) 2014, Tamas Csala

#version 120

varying vec2 vTexCoord;

uniform vec4 uBgColor, uBorderColor;
uniform vec2 uBorderWidth;

bool isBorder() {
  vec2 dist_from_border = abs(0.5 - abs(vTexCoord-vec2(0.5)));
  return dist_from_border.x < uBorderWidth.x || dist_from_border.y < uBorderWidth.y;
}

void main() {
  gl_FragColor = isBorder() ? uBorderColor : uBgColor;
}

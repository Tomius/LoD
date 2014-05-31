// Copyright (c) 2014, Tamas Csala

#version 120

varying vec2 vTexCoord;

uniform vec4 uBgColor, uBorderColor;
uniform vec2 uBorderWidth;
uniform float uBorderPixels;
uniform vec2 uCorners[4];

#define CheckCornerMacro(corner_num)                                      \
  float len = length(gl_FragCoord.xy - uCorners[corner_num]);             \
  if(len > 10 - uBorderPixels) {                                          \
    if(len > 10) {                                                        \
      discard;                                                            \
    } else {                                                              \
      return true;                                                        \
    }                                                                     \
  }

bool CheckCorner() {
  if(gl_FragCoord.x < uCorners[0].x && gl_FragCoord.y < uCorners[0].y) {
    CheckCornerMacro(0);
  }
  if(gl_FragCoord.x < uCorners[1].x && gl_FragCoord.y > uCorners[1].y) {
    CheckCornerMacro(1);
  }
  if(gl_FragCoord.x > uCorners[2].x && gl_FragCoord.y < uCorners[2].y) {
    CheckCornerMacro(2);
  }
  if(gl_FragCoord.x > uCorners[3].x && gl_FragCoord.y > uCorners[3].y) {
    CheckCornerMacro(3);
  }

  return false;
}

bool IsBorder() {
  // distance from the nearest borders
  vec2 dist = abs(0.5 - abs(vTexCoord-vec2(0.5)));
  return CheckCorner() || dist.x < uBorderWidth.x || dist.y < uBorderWidth.y;
}

void main() {
  gl_FragColor = IsBorder() ? uBorderColor : uBgColor;
}

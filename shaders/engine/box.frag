// Copyright (c) 2014, Tamas Csala

#version 120

varying vec2 vTexCoord;

uniform vec4 uBgColor, uBorderColor;
uniform vec2 uBorderWidth;
uniform vec2 uCorners[4];

uniform vec4 uBgTopColor, uBgTopMidColor, uBgBottomMidColor, uBgBottomColor;
uniform float uBorderPixels, uTransitionHeight, uRoundness, uInverted;

#define CheckCornerMacro(corner_num)                                      \
  float len = length(gl_FragCoord.xy - uCorners[corner_num]);             \
  if(len > uRoundness - uBorderPixels) {                                  \
    if(len > uRoundness) {                                                \
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
  // distances from the nearest edges
  vec2 dist = abs(0.5 - abs(vTexCoord-vec2(0.5)));
  return CheckCorner() || dist.x < uBorderWidth.x || dist.y < uBorderWidth.y;
}

vec4 bgColor() {
  if (uTransitionHeight < 0) {
    return uBgColor;
  } else {
    if (vTexCoord.y > uTransitionHeight) {
      return mix(uBgTopColor, uBgTopMidColor,
                (1 - vTexCoord.y) / (1 - uTransitionHeight));
    } else {
      return mix(uBgBottomColor, uBgBottomMidColor, vTexCoord.y / uTransitionHeight);
    }
  }
}

void main() {
  gl_FragColor = IsBorder() ? uBorderColor : bgColor();
}

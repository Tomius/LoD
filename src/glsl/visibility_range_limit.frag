// Copyright (c) 2014, Tamas Csala

#version 430

#export float VisibilityRangeAlpha(vec3 c_pos);

uniform float uMaxOpaqueDist = 900, uMaxVisibleDist = 1000;

float VisibilityRangeAlpha(vec3 c_pos) {
  float l = length(c_pos);
  if (l > uMaxVisibleDist) {
    discard;
  } else if (l > uMaxOpaqueDist) {
    return 1 - (l - uMaxOpaqueDist) / (uMaxVisibleDist - uMaxOpaqueDist);
  } else {
    return 1.0;
  }
}

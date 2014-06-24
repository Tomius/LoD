// Copyright (c) 2014, Tamas Csala

#version 120

#include "sky.frag"

#export vec3 HemisphereLighting(vec3 w_normal);

vec3 HemisphereLighting(vec3 w_normal) {
  vec3 ground_color = vec3(0.1) + SunPower()*SunColor()/4 + MoonPower()*MoonColor()/8;
  vec3 sky_color = 3*ground_color + vec3(0.1, 0.1, 0.3);

  return mix(ground_color, sky_color, clamp(w_normal.y, 0, 1));
}

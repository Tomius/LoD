// Copyright (c) 2014, Tamas Csala

#version 120

#include "sky.frag"
#include "fog.frag"

varying vec3 c_vPos;
varying vec3 w_vNormal;
varying vec2 vTexCoord;

uniform sampler2D uDiffuseTexture;

const float kMaxVisibleDist = 1450.0;
const float kMaxOpaqueDist = 1350.0;
vec3 kLightColor;

void main() {
  float alpha = 1.0, l = length(c_vPos);
  if (l > kMaxVisibleDist) {
    discard;
  } else if (l > kMaxOpaqueDist) {
    alpha = 1 - (l - kMaxOpaqueDist) / (kMaxVisibleDist - kMaxOpaqueDist);
  }

  float diffuse_power = max(abs(dot(normalize(w_vNormal), SunPos())), 0.2);
  vec3 lighting;
  vec3 w_sun_dir = SunPos();

  vec4 color = texture2D(uDiffuseTexture, vTexCoord);

  vec3 ground_color = vec3(0.1) + SunPower()*SunColor()/4 + MoonPower()*MoonColor()/8;
  vec3 sky_color = 3*ground_color + vec3(0.1, 0.1, 0.3);

  lighting = mix(ground_color, sky_color, clamp(w_vNormal.y, 0, 1));

  vec3 final_color = color.rgb * lighting;
  float actual_alpha = min(color.a, alpha);
  if (actual_alpha < 1e-3) { discard; }

  gl_FragColor = vec4(ApplyFog(final_color, c_vPos), actual_alpha);
}

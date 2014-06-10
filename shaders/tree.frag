// Copyright (c) 2014, Tamas Csala

#version 120

varying vec3 c_vPos;
varying vec3 w_vNormal;
varying vec2 vTexCoord;

uniform sampler2D uDiffuseTexture;

vec3 SunPos();
vec3 MoonPos();
float SunPower();
float MoonPower();
vec3 SunColor();
vec3 MoonColor();
vec3 AmbientColor();
float AmbientPower();

const float kFogMin = 128.0;
const float kFogMax = 2048.0;
const float kMaxVisibleDist = 1450.0;
const float kMaxOpaqueDist = 1350.0;

void main() {
  float alpha = 1.0, l = length(c_vPos);
  if (l > kMaxVisibleDist) {
    discard;
  } else if (l > kMaxOpaqueDist) {
    alpha = 1 - (l - kMaxOpaqueDist) / (kMaxVisibleDist - kMaxOpaqueDist);
  }

  float diffuse_power = max(abs(dot(normalize(w_vNormal), SunPos())), 0.2);

  vec4 color = texture2D(uDiffuseTexture, vTexCoord);
  // FIXME
  vec3 final_color = color.rgb * (vec3(0.1) + 0.9*AmbientColor()) *
                     (diffuse_power + AmbientPower()/2);

  float actual_alpha = min(color.a, alpha);

  if (actual_alpha < 1e-3) {
    discard;
  }

  float length_from_camera = length(c_vPos);

  vec3 fog_color = AmbientColor() / 3;
  float fog_alpha = clamp((length_from_camera - kFogMin) /
                          (kFogMax - kFogMin), 0.0, 1.0) / 6;

  gl_FragColor = vec4(mix(final_color,
                          fog_color, fog_alpha), actual_alpha);
}

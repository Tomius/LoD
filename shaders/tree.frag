// Copyright (c) 2014, Tamas Csala

#version 120

varying vec3 c_vPos;
varying vec3 w_vNormal;
varying vec2 vTexcoord;

uniform sampler2D uDiffuseTexture;

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
float isDay();

float kFogMin = 128.0;
float kFogMax = 2048.0;

void main() {
  float diffuse_power = max(abs(dot(normalize(w_vNormal), normalize(AmbientDirection()))), 0.3);

  vec4 color = texture2D(uDiffuseTexture, vTexcoord);
  vec3 final_color = color.rgb * (SunPower() * diffuse_power + AmbientPower()) / 2;

  float actual_alpha = color.a;

  if (actual_alpha < 1e-3) {
    discard;
  }

  float length_from_camera = length(c_vPos);

  vec3 fog_color = vec3(mix(-1.6, 0.8, isDay()));
  vec3 fog = fog_color * (0.005 + SunPower());
  float fog_alpha = clamp((length_from_camera - kFogMin) / (kFogMax - kFogMin), 0.0, 1.0) / 6;

  gl_FragColor = vec4(mix(pow(final_color, vec3(1.3)), fog, fog_alpha), actual_alpha);
}

// Copyright (c) 2014, Tamas Csala

#version 430

#include "sky.frag"
#include "hemisphere_lighting.frag"

in vec3 w_vNormal, c_vNormal;
in vec3 w_vPos, c_vPos;
in vec2 vTexCoord;

uniform mat4 uCameraMatrix;
uniform sampler2D uDiffuseTexture, uSpecularTexture;

out vec4 fragColor;

const float kSpecularShininess = 20.0f;

void CalculateLighting(vec3 c_light_dir, out float diffuse_power,
                       out float specular_power) {
  vec3 c_normal = normalize(c_vNormal);
  vec3 c_view_dir = normalize(-c_vPos);

  diffuse_power = dot(c_normal, c_light_dir);
  if (diffuse_power <= 0.0) {
    diffuse_power = 0.0;
    specular_power = 0.0;
  } else {
    vec3 L = c_light_dir, V = c_view_dir;
    vec3 H = normalize(L + V), N = c_normal;
    specular_power = pow(max(dot(H, N), 0), kSpecularShininess);
  }
}

void main() {
  float spec_mask = texture2D(uSpecularTexture, vTexCoord).r;

  vec3 lighting = HemisphereLighting(normalize(w_vNormal));
  vec3 w_sun_dir = SunPos();

  if (w_sun_dir.y > 0) {
    float diffuse_power, specular_power;
    vec3 c_sun_dir = mat3(uCameraMatrix) * w_sun_dir;
    CalculateLighting(c_sun_dir, diffuse_power, specular_power);
    diffuse_power *= pow(SunPower(), 0.3);
    specular_power *= pow(SunPower(), 0.3);
    lighting += SunColor() * (diffuse_power + spec_mask*specular_power);
  } else {
    float diffuse_power, specular_power;
    vec3 c_moon_dir = mat3(uCameraMatrix) * -w_sun_dir;
    CalculateLighting(c_moon_dir, diffuse_power, specular_power);
    diffuse_power *= pow(MoonPower(), 0.3);
    specular_power *= pow(MoonPower(), 0.3);
    lighting += MoonColor() * (diffuse_power + spec_mask*specular_power);
  }

  vec3 diffuse_color = texture2D(uDiffuseTexture, vTexCoord).rgb;
  vec3 final_color = diffuse_color * lighting;

  fragColor = vec4(final_color, 1.0);
}

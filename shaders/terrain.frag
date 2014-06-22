// Copyright (c) 2014, Tamas Csala

#version 120

#include "sky.frag"

// This might be overwritten by the c++ code.
#define SHADOW_MAP_NUM 16

varying vec3  w_vNormal;
varying vec3  c_vPos, w_vPos;
varying vec2  vTexCoord;
varying float vInvalid;
varying mat3  vNormalMatrix;

uniform mat4 uCameraMatrix;
uniform sampler2D uGrassMap0, uGrassMap1, uGrassNormalMap;
uniform sampler2DShadow uShadowMap;

uniform mat4 uShadowCP[SHADOW_MAP_NUM];
uniform int uNumUsedShadowMaps;
uniform ivec2 uShadowAtlasSize;

// We love #version 120
int min(int a, int b) {
  return a > b ? b : a;
}

// -------======{[ Shadow ]}======-------

// The maximum potion of light that should be subtracted
// if the object is in shadow. For ex. 0.8 means, object in
// shadow is 20% as bright as a lit one.
const float kMaxShadow = 0.8;

vec2 GetShadowAtlasOffset(int i) {
  return vec2(i / uShadowAtlasSize.x, mod(i, uShadowAtlasSize.y));
}

vec2 AtlasLookup(vec2 tc, int i) {
  return (tc + GetShadowAtlasOffset(i)) / uShadowAtlasSize;
}

bool isValid(vec2 tc) {
  return 0 <= tc.x && tc.x <= 1 && 0 <= tc.y && tc.y <= 1;
}

float Visibility() {
  float visibility = 1.0;
  int num_shadow_casters = min(uNumUsedShadowMaps, SHADOW_MAP_NUM);
  float length_from_camera = length(c_vPos);
  float modifier = max((300.0 - length_from_camera) / 300.0, 0.0);
  modifier *= kMaxShadow;

  // For every shadow casters
  for (int i = 0; i < num_shadow_casters; ++i) {
    vec4 shadowCoord = uShadowCP[i] * vec4(w_vPos, 1.0);

    if (!isValid(shadowCoord.xy)) {
      continue;
    }

    visibility -= modifier * (1 - shadow2D(
      uShadowMap,
      vec3(
        AtlasLookup(shadowCoord.xy, i),
        (shadowCoord.z) / shadowCoord.w
      )
    ).r);
  }

  return max(visibility, 0.0);
}

const float kFogMin = 128.0;
const float kFogMax = 2048.0;
const float kSpecularShininess = 64.0;

void CalculateLighting(vec3 c_normal, vec3 c_light_dir,
                       out float diffuse_power, out float specular_power) {
  vec3 c_view_dir = -normalize((uCameraMatrix * vec4(w_vPos, 1)).xyz);

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
  if (vInvalid != 0.0) {
    discard;
  }

  // Normals
  mat3 normal_matrix;
  normal_matrix[0] = normalize(vNormalMatrix[0]);
  normal_matrix[1] = normalize(vNormalMatrix[1]);
  normal_matrix[2] = normalize(vNormalMatrix[2]);
  vec3 normal_offset = texture2D(uGrassNormalMap, vTexCoord*256).rgb;
  vec3 w_normal = normalize(normal_matrix[2] + normal_matrix * normal_offset);
  vec3 c_normal = mat3(uCameraMatrix) * w_normal;

  // Lighting
  vec3 lighting;
  vec3 w_sun_dir = SunPos();
  if (w_sun_dir.y > 0) {
    float diffuse_power, specular_power;
    vec3 c_sun_dir = mat3(uCameraMatrix) * w_sun_dir;
    CalculateLighting(c_normal, c_sun_dir, diffuse_power, specular_power);
    diffuse_power *= pow(SunPower(), 0.3);
    specular_power *= pow(SunPower(), 0.3);
    lighting = SunColor() * (diffuse_power + specular_power + 1.5f*AmbientPower() + 0.1);
  } else {
    float diffuse_power, specular_power;
    vec3 c_moon_dir = mat3(uCameraMatrix) * -w_sun_dir;
    CalculateLighting(c_normal, c_moon_dir, diffuse_power, specular_power);
    diffuse_power *= pow(MoonPower(), 0.3);
    specular_power *= pow(MoonPower(), 0.3);
    lighting = MoonColor() * (diffuse_power + specular_power + 1.5f*AmbientPower() + 0.1);
  }

  vec3 grass_color_0 = texture2D(uGrassMap0, vTexCoord*256).rgb;
  vec3 grass_color_1 = texture2D(uGrassMap0, vTexCoord*16).rgb;

  vec3 rock_color_0 = texture2D(uGrassMap1, vTexCoord*256).rgb;
  vec3 rock_color_1 = texture2D(uGrassMap1, vTexCoord*16).rgb;

  float height_factor = clamp(sqrt(max(w_vPos.y - 64, 0) / 128), 0, 1);

  vec3 color_0 = mix(grass_color_0, rock_color_0, height_factor);
  vec3 color_1 = mix(grass_color_1, rock_color_1, height_factor/2);
  vec3 diffuse_color = mix(color_0, color_1, 0.5);

  float visibility = Visibility();
  if(w_sun_dir.y > 0) {
    visibility = max(pow(visibility, pow(SunPower(), 0.2)), 0.2);
  } else {
    visibility = max(pow(visibility, pow(MoonPower(), 0.2)), 0.2);
  }
  vec3 final_color = diffuse_color * (visibility + AmbientPower()) * lighting;

  // Fog
  vec3 fog_color = AmbientColor() / 3;
  float length_from_camera = length(c_vPos);
  float alpha = clamp((length_from_camera - kFogMin) /
                      (kFogMax - kFogMin), 0, 1) / 4;

  gl_FragColor = vec4(mix(final_color, fog_color, alpha), 1);
}

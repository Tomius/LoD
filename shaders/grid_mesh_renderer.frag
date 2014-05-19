// Copyright (c) 2014, Tamas Csala

#version 120

varying vec3  w_vNormal;
varying vec3  c_vPos, w_vPos;
varying vec2  vTexcoord;
varying float vInvalid;
varying mat3  vNormalMatrix;

uniform mat4 uCameraMatrix;
uniform sampler2D uGrassMap0, uGrassMap1, uGrassNormalMap;

// External functions
vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();
float isDay();

float kFogMin = 128.0;
float kFogMax = 2048.0;

const float kSpecularShininess = 20.0;

void main() {
  if (vInvalid != 0.0) {
    discard;
  }

  // TexCoord
  vec2 grass_texcoord = vTexcoord * 200;

  // Normals
  vec3 w_normal = normalize(w_vNormal);
  vec3 normal_offset = texture2D(uGrassNormalMap, grass_texcoord).rgb;
  vec3 w_final_normal = normalize(vNormalMatrix * normal_offset);
  vec3 c_normal = (uCameraMatrix * vec4(w_final_normal, 0.0)).xyz;

  // Lighting directions
  vec3 c_light_dir = normalize((uCameraMatrix * vec4(AmbientDirection(), 0)).xyz);
  vec3 c_view_direction = normalize(-(uCameraMatrix * vec4(w_vPos, 1)).xyz);

  // Lighting values
  float diffuse_power = dot(c_normal, c_light_dir);
  float specular_power;
  if (diffuse_power <= 0.0) {
    diffuse_power = 0;
    specular_power = 0;
  } else {
    vec3 L = c_light_dir, V = c_view_direction;
    vec3 H = normalize(L + V), N = c_normal;
    specular_power = 0.5 * pow(max(dot(H, N), 0), kSpecularShininess);
  }

  // Colors
  vec3 grass0_color = texture2D(uGrassMap0, grass_texcoord).rgb;
  vec3 grass1_color = texture2D(uGrassMap1, grass_texcoord).rgb;
  vec3 grass10_color = texture2D(uGrassMap0, grass_texcoord/16).rgb;
  vec3 grass11_color = texture2D(uGrassMap1, grass_texcoord/16).rgb;
  vec3 grass110_color = texture2D(uGrassMap0, grass_texcoord/64).rgb;
  vec3 grass111_color = texture2D(uGrassMap1, grass_texcoord/64).rgb;
  float height_factor = clamp(sqrt((w_vPos.y - 15) / 40), 0, 1);
  vec3 grass_color0 = mix(grass0_color, grass1_color, height_factor);
  vec3 grass_color1 = mix(grass10_color, grass11_color, height_factor);
  vec3 grass_color11 = mix(grass110_color, grass111_color, height_factor);
  vec3 grass_color = mix(grass_color0, mix(grass_color1, grass_color11, 0.5), 0.5);

  float length_from_camera = length(c_vPos);

  const float ambient_occlusion = 0.2f;

  vec3 final_color = grass_color * AmbientColor() *
    (SunPower()*(specular_power + diffuse_power + ambient_occlusion) + AmbientPower());

  // Fog
  vec3 fog_color = vec3(mix(-1.6f, 0.8f, isDay()));
  vec3 fog = AmbientColor() * fog_color * (0.005 + SunPower());
  float alpha = clamp((length_from_camera - kFogMin) / (kFogMax - kFogMin), 0, 1) / 4;

  gl_FragColor = vec4(mix(pow(final_color, vec3(0.7)), fog, alpha), 1);
}

// Copyright (c) 2014, Tamas Csala

#version 120

varying vec3 c_vNormal;
varying vec3 w_vPos, c_vPos;
varying vec2 vTexCoord;

uniform mat4 uCameraMatrix;
uniform sampler2D uDiffuseTexture, uSpecularTexture;

vec3 SunPos();
vec3 MoonPos();
float SunPower();
float MoonPower();
vec3 SunColor();
vec3 MoonColor();
vec3 AmbientColor();
float AmbientPower();

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
    specular_power = pow(
      max(
        dot(
          reflect(-c_light_dir, c_normal),
          c_view_dir
        ), 0.0
      ), kSpecularShininess
    );
  }
}

void main() {
  float spec_mask = texture2D(uSpecularTexture, vTexCoord).r;

  vec3 w_sun_dir = SunPos();
  vec3 lighting;

  if (w_sun_dir.y > 0) {
    float diffuse_power, specular_power;
    vec3 c_sun_dir = mat3(uCameraMatrix) * w_sun_dir;
    CalculateLighting(c_sun_dir, diffuse_power, specular_power);
    diffuse_power *= 0.1 + 0.9*SunPower();
    specular_power *= 0.1 + 0.9*SunPower();
    lighting = SunColor() * (diffuse_power + spec_mask*specular_power);
  } else {
    float diffuse_power, specular_power;
    vec3 c_moon_dir = mat3(uCameraMatrix) * -w_sun_dir;
    CalculateLighting(c_moon_dir, diffuse_power, specular_power);
    diffuse_power *= 0.1 + 0.9*MoonPower();
    specular_power *= 0.1 + 0.9*MoonPower();
    lighting = MoonColor() * (diffuse_power + spec_mask*specular_power);
  }

  vec3 diffuse_color = texture2D(uDiffuseTexture, vTexCoord).rgb;
  vec3 final_color = diffuse_color * (lighting + AmbientPower());

  gl_FragColor = vec4(final_color, 1.0);
}

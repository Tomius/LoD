// Copyright (c) 2014, Tamas Csala

#version 120

varying vec3 c_vNormal;
varying vec3 w_vPos, c_vPos;
varying vec2 vTexCoord;

uniform mat4 uCameraMatrix;
uniform sampler2D uDiffuseTexture, uSpecularTexture;

vec3 AmbientDirection();
float AmbientPower();
float SunPower();

const float kSpecularShininess = 20.0f;

void main() {
  vec3 c_normal = normalize(c_vNormal);
  vec3 c_viewDir = normalize(-c_vPos);

  vec3 c_lightDir = normalize((uCameraMatrix * vec4(AmbientDirection(), 0)).xyz);
  float diffuse_power = dot(c_normal, c_lightDir);

  float specular_power;
  if (diffuse_power <= 0.0) {
    diffuse_power = 0.0;
    specular_power = 0.0;
  } else {
    specular_power = pow(
      max(
        dot(
          reflect(-c_lightDir, c_normal),
          c_viewDir
        ), 0.0
      ), kSpecularShininess
    );
  }

  vec3 color = texture2D(uDiffuseTexture, vTexCoord).rgb;
  float spec_mask = texture2D(uSpecularTexture, vTexCoord).r;

  vec3 final_color = color *
    (SunPower()*(diffuse_power + spec_mask*specular_power) + (AmbientPower() + 0.1));

  gl_FragColor = vec4(final_color, 1.0);
}

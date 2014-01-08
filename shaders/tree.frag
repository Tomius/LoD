#version 150

in vec3 c_vPos;
in vec3 w_vNormal;
in vec2 vTexcoord;

uniform sampler2D uDiffuseTexture;
uniform vec3 uScales = vec3(0.5, 0.5, 0.5);

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();
float isDay();

float xz_scale = sqrt(uScales.x*uScales.z);
float kMaxVisibleDist = xz_scale * 800.0;
float kMaxOpaqueDist = xz_scale * 700.0;
float kFogMin = xz_scale * 128.0;
float kFogMax = xz_scale * 2048.0;

out vec4 vFragColor;

void main() {
  float alpha = 1.0, l = length(c_vPos);
  if(l > kMaxVisibleDist) {
    discard;
  } else if (l > kMaxOpaqueDist) {
    alpha = 1 - (l - kMaxOpaqueDist) / (kMaxVisibleDist - kMaxOpaqueDist);
  }

  float diffuse_power = max(abs(dot(normalize(w_vNormal), normalize(AmbientDirection()))), 0.3);

  vec4 color = texture(uDiffuseTexture, vTexcoord);
  vec3 final_color = color.rgb * AmbientColor() * (SunPower() * diffuse_power + AmbientPower()) / 2;

  float actual_alpha = min(color.a, alpha);

  if(actual_alpha < 1e-3) {
    discard;
  }

  float length_from_camera = length(c_vPos);

  vec3 fog_color = vec3(mix(-1.6f, 0.8f, isDay()));
  vec3 fog = AmbientColor() * fog_color * (0.005 + SunPower());
  float fog_alpha = clamp((length_from_camera - kFogMin) / (kFogMax - kFogMin), 0.0, 1.0) / 6;

  vFragColor = vec4(mix(pow(final_color, vec3(1.3)), fog, fog_alpha), actual_alpha);
}

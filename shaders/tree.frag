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

float kMaxVisibleDist = max(uScales.x, uScales.z) * 800.0;
float kMaxOpaqueDist = max(uScales.x, uScales.z) * 700.0;

out vec4 vFragColor;

void main() {
  float alpha = 1.0, l = length(c_vPos);
  if(l > kMaxVisibleDist) {
    discard;
  } else if (l > kMaxOpaqueDist) {
    alpha = 1 - (l - kMaxOpaqueDist) / (kMaxVisibleDist - kMaxOpaqueDist);
  }

  float diffuse_power = max(
    dot(normalize(w_vNormal), normalize(AmbientDirection())),
    0.2
  );

  vec4 color = texture(uDiffuseTexture, vTexcoord);
  vec3 final_color = color.rgb * AmbientColor() * (SunPower() * diffuse_power + AmbientPower()) / 2;

  float actual_alpha = min(color.a, alpha);

  if(actual_alpha < 1e-3) {
    discard;
  }

  vFragColor = vec4(pow(final_color, vec3(1.3)), actual_alpha);
}

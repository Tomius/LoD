#version 150

in vec3 vTexcoord;

out vec4 vFragColor;

vec3 SkyColor(vec3 lookDir);

void main() {
  vFragColor = vec4(SkyColor(normalize(vTexcoord)), 1.0);
}

#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

varying vec3 vTexcoord;

vec3 SkyColor(vec3 lookDir);

void main() {
  gl_FragColor = vec4(SkyColor(normalize(vTexcoord)), 1.0);
}

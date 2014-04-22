#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

varying vec2 vTexcoord;

uniform sampler2D uDiffuseTexture;

void main() {
  if(texture2D(uDiffuseTexture, vTexcoord).a < 1.0)
    discard;
}

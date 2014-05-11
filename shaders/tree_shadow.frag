#version 120

varying vec2 vTexcoord;

uniform sampler2D uDiffuseTexture;

void main() {
  if (texture2D(uDiffuseTexture, vTexcoord).a < 1.0)
    discard;
}

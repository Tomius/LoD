#version 150

in vec2 vTexcoord;

uniform sampler2D uDiffuseTexture;

void main() {
  if(texture(uDiffuseTexture, vTexcoord).a < 1.0)
    discard;
}

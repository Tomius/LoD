#version 330 core

in VertexData {
    vec2 texcoord;
} vin;

uniform sampler2D uDiffuseTexture;

void main() {
  if(texture(uDiffuseTexture, vin.texcoord).a < 1.0)
    discard;
}

#version 140
#extension GL_ARB_gpu_shader5 : enable

in VertexData {
  vec2 texcoord;
} vin;

uniform sampler2D uDiffuseTexture;

void main() {
  if(texture(uDiffuseTexture, vin.texcoord).a < 1.0)
    discard;
}

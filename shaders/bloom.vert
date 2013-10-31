#version 140
#extension GL_ARB_gpu_shader5 : enable

in vec4 vPosition;

void main() {
  gl_Position = vPosition;
}

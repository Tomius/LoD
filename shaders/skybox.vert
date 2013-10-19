#version 330 core

in vec3 vPosition;

uniform mat4 uProjectionMatrix;
uniform mat3 uCameraMatrix;

out VertexData {
  vec3 texcoord;
};

void main(void) {
  gl_Position = uProjectionMatrix * vec4(uCameraMatrix * vec3(10 * vPosition), 1.0);
  texcoord = vPosition;
}

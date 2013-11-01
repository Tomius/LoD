#version 140

in vec3 aPosition;

uniform mat4 uProjectionMatrix;
uniform mat3 uCameraMatrix;

out vec3 vTexcoord;

void main(void) {
  gl_Position = uProjectionMatrix * vec4(uCameraMatrix * vec3(10 * aPosition), 1.0);
  vTexcoord = aPosition;
}

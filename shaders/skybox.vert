#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

attribute vec3 aPosition;

uniform mat4 uProjectionMatrix;
uniform mat3 uCameraMatrix;

varying vec3 vTexcoord;

void main(void) {
  gl_Position = uProjectionMatrix * vec4(uCameraMatrix * vec3(10 * aPosition), 1.0);
  vTexcoord = aPosition;
}

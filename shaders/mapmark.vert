#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

attribute vec2 aPosition;
attribute vec2 aTexcoord;

uniform mat3 uModelMatrix;

varying vec2 vTexcoord;

void main() {
	vTexcoord = aTexcoord;
	gl_Position = vec4((uModelMatrix*vec3(aPosition, 1)).xy, 0, 1);
}

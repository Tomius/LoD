#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

attribute vec2 aPosition;
attribute vec2 aTexcoord;

varying vec2 vTexcoord;

void main() {
	vTexcoord = aTexcoord;
	gl_Position = vec4(0.6*aPosition, 0, 1); // I want the map to fill only ~half of the screen
}

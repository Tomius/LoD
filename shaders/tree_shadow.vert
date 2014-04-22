#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

attribute vec4 aPosition;
attribute vec2 aTexCoord;

uniform mat4 uMCP;

varying vec2 vTexcoord;

void main() {
  vTexcoord = aTexCoord;
  gl_Position = uMCP * aPosition;
}

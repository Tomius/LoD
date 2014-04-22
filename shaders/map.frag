#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

varying vec2 vTexcoord;

uniform sampler2D uTex;

void main() {
	gl_FragColor = vec4(texture2D(uTex, vTexcoord).rgb, 0.6);
}

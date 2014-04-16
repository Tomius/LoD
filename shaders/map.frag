#version 120

varying vec2 vTexcoord;

uniform sampler2D uTex;

void main() {
	gl_FragColor = vec4(texture2D(uTex, vTexcoord).rgb, 0.6);
}

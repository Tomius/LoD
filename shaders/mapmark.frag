#version 120

varying vec2 vTexcoord;

uniform sampler2D uTex;

void main() {
	gl_FragColor = texture2D(uTex, vTexcoord) * vec4(1, 1, 1, 0.5);
}

#version 150

in vec2 vTexcoord;
uniform sampler2D uTex; 

out vec4 vFragColor;

void main() {
	vFragColor = texture(uTex, vTexcoord).rgba * vec4(1, 1, 1, 0.5);
}
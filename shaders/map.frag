#version 150

in vec2 vTexcoord;
uniform sampler2D uTex; 

out vec4 vFragColor;

void main() {
	vFragColor = vec4(texture(uTex, vTexcoord).rgb, 0.6);
}
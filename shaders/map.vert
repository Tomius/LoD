#version 120

attribute vec2 aPosition;
attribute vec2 aTexcoord;

varying vec2 vTexcoord;

void main() {
	vTexcoord = aTexcoord;
	gl_Position = vec4(0.6*aPosition, 0, 1); // I want the map to fill only ~half of the screen
}

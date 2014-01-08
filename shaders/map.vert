#version 150

in vec2 aPosition; 
in vec2 aTexcoord;

out vec2 vTexcoord;

void main() {
	vTexcoord = aTexcoord;
	gl_Position = vec4(0.6*aPosition, 0, 1); // I want the map to fill only ~half of the screen
}
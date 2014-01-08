#version 150

in vec2 aPosition; 
in vec2 aTexcoord;

uniform mat3 uModelMatrix;

out vec2 vTexcoord;

void main() {
	vTexcoord = aTexcoord;
	gl_Position = vec4((uModelMatrix*vec3(aPosition, 1)).xy, 0, 1);
}
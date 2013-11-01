#version 140

in vec4 aPosition;
in vec2 aTexCoord;

uniform mat4 uMCP;

out vec2 vTexcoord;

void main() {
  vTexcoord = aTexCoord;
  gl_Position = uMCP * aPosition;
}

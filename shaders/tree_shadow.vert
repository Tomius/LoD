#version 330 core

in vec4 vPosition;
in vec2 vTexCoord;

uniform mat4 uMCP;

out VertexData {
    vec2 texcoord;
} vout;

void main() {
    vout.texcoord = vTexCoord;
    gl_Position = uMCP * vPosition;
}

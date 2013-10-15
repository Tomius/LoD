#version 330 core

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;

out VertexData {
    vec3 c_pos;
    vec3 w_normal;
    vec2 texcoord;
} vout;

void main() {
    vout.w_normal = vNormal * mat3(inverse(transpose(uModelMatrix)));
    vout.texcoord = vTexCoord;

    vec4 c_pos = uCameraMatrix * (uModelMatrix * (vPosition * vec4(2, 2, 2, 1)));
    vout.c_pos = vec3(c_pos);

    gl_Position = uProjectionMatrix * c_pos;
}

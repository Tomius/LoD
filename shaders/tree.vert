#version 330 core

in vec4 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;

uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;

out VertexData {
    vec3 c_pos;
    vec3 w_normal;
    vec2 texcoord;
} vout;

void main() {
    vout.w_normal = vNormal * mat3(inverse(transpose(uModelMatrix)));
    vout.texcoord = vTexCoord;

    vec4 c_pos = uCameraMatrix * (uModelMatrix * vPosition);
    vout.c_pos = vec3(c_pos);

    gl_Position = uProjectionMatrix * c_pos;
}

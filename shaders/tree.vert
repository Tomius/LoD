#version 330 core

in vec4 a_Position;
in vec3 a_Normal;
in vec2 a_TexCoord;

uniform mat4 u_ProjectionMatrix, u_CameraMatrix, u_ModelMatrix;

out VertexData {
    vec3 c_Pos;
    vec3 w_Normal;
    vec2 m_TexCoord;
} vert;

void main() {
    vert.w_Normal = mat3(inverse(transpose(u_ModelMatrix))) * a_Normal;
    vert.m_TexCoord = a_TexCoord;
    vec4 cameraPos = u_CameraMatrix * (u_ModelMatrix * (a_Position * vec4(2, 2, 2, 1)));
    vert.c_Pos = vec3(cameraPos);

    gl_Position =
        u_ProjectionMatrix * cameraPos;
}

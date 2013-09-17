#version 330 core

in VertexData {
    vec3 c_Pos;
    vec3 w_Normal;
    vec2 m_TexCoord;
} vert;

uniform sampler2D u_DiffuseTexture;
uniform vec3 Scales = vec3(0.5, 0.5, 0.5);

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();

float maxDist = max(Scales.x, Scales.z) * 512.0;

out vec4 fragColor;

void main() {
    float l = length(vert.c_Pos);
    if(l > maxDist)
        discard;

    float diffuse_power =
        max(
            dot(
                normalize(vert.w_Normal),
                normalize(AmbientDirection())),
            0
        );

    vec4 color = texture(u_DiffuseTexture, vert.m_TexCoord);
    vec3 finalColor = color.rgb * AmbientColor() * (SunPower() * diffuse_power + AmbientPower());

    fragColor = vec4(pow(finalColor, vec3(1.5)), color.a);
}

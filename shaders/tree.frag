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

float maxVisibleDist = max(Scales.x, Scales.z) * 800.0;
float maxOpaqueDist = max(Scales.x, Scales.z) * 700.0;

out vec4 fragColor;

void main() {
    float alpha = 1.0, l = length(vert.c_Pos);
    if(l > maxVisibleDist) {
        discard;
    } else if (l > maxOpaqueDist) {
        alpha = 1 - (l - maxOpaqueDist) / (maxVisibleDist - maxOpaqueDist);
    }

    float diffuse_power =
        max(
            dot(
                normalize(vert.w_Normal),
                normalize(AmbientDirection())),
            0.2
        );

    vec4 color = texture(u_DiffuseTexture, vert.m_TexCoord);
    vec3 finalColor = color.rgb * AmbientColor() * (SunPower() * diffuse_power + AmbientPower()) / 2;

    fragColor = vec4(pow(finalColor, vec3(1.3)), min(color.a, alpha));
}

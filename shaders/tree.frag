#version 330 core

in VertexData {
    vec3 c_pos;
    vec3 w_normal;
    vec2 m_texcoord;
} vin;

uniform sampler2D uDiffuseTexture;
uniform vec3 uScales = vec3(0.5, 0.5, 0.5);

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();

float kMaxVisibleDist = max(uScales.x, uScales.z) * 800.0;
float kMaxOpaqueDist = max(uScales.x, uScales.z) * 700.0;

out vec4 frag_color;

void main() {
    float alpha = 1.0, l = length(vin.c_pos);
    if(l > kMaxVisibleDist) {
        discard;
    } else if (l > kMaxOpaqueDist) {
        alpha = 1 - (l - kMaxOpaqueDist) / (kMaxVisibleDist - kMaxOpaqueDist);
    }

    float diffuse_power =
        max(
            dot(
                normalize(vin.w_normal),
                normalize(AmbientDirection())),
            0.2
        );

    vec4 color = texture(uDiffuseTexture, vin.m_texcoord);
    vec3 final_color = color.rgb * AmbientColor() * (SunPower() * diffuse_power + AmbientPower()) / 2;

    frag_color = vec4(pow(final_color, vec3(1.3)), min(color.a, alpha));
}

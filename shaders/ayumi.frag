#version 330 core

in VertexData {
    vec3 c_normal;
    vec3 c_pos;
    vec2 m_texCoord;
} vin;

uniform sampler2D uDiffuseTexture, uSpecularTexture;
uniform mat4 uCameraMatrix;

out vec4 frag_color;

vec3 AmbientDirection();
float AmbientPower();
float SunPower();
vec3 AmbientColor();

const float kSpecularShininess = 20.0f;

void main() {
    vec3 c_normal = normalize(vin.c_normal);
    vec3 c_viewDir = normalize(-vin.c_pos);

    vec3 c_lightDir = normalize((uCameraMatrix * vec4(AmbientDirection(), 0)).xyz);
    float diffuse_power = max(dot(c_normal, c_lightDir), 0);

    float specular_power;
    if(diffuse_power < 0.0) {
        specular_power = 0.0;
    } else {
        specular_power = pow(
            max(
                dot(
                    reflect(-c_lightDir, c_normal),
                    c_viewDir),
                0.0),
            kSpecularShininess
        );
    }

    vec3 color = texture(uDiffuseTexture, vin.m_texCoord).rgb;
    float spec_mask = texture(uSpecularTexture, vin.m_texCoord).r;

    vec3 final_color = color * AmbientColor() *
        (SunPower() * (diffuse_power + spec_mask * specular_power) + AmbientPower());

    frag_color = vec4(final_color, 1.0);
}

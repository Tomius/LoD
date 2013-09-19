#version 330 core

in VertexData {
    vec3 c_normal;
    vec3 c_pos;
    vec2 texCoord;
} vert;

uniform sampler2D DiffuseTexture, SpecularTexture;
uniform mat4 CameraMatrix;

out vec4 fragColor;

vec3 AmbientDirection();
float AmbientPower();
float SunPower();
vec3 AmbientColor();

const float specular_shininess = 20.0f;

void main() {
    vec3 c_normal = normalize(vert.c_normal);
    vec3 c_viewDir = normalize(-vert.c_pos);

    vec3 c_lightDir = normalize((CameraMatrix * vec4(AmbientDirection(), 0)).xyz);
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
            specular_shininess
        );
    }

    vec3 color = texture(DiffuseTexture, vert.texCoord).rgb;
    float spec_mask = texture(SpecularTexture, vert.texCoord).r;

    vec3 finalColor = color * AmbientColor() *
        (SunPower() * (diffuse_power + spec_mask * specular_power) + AmbientPower());

    fragColor = vec4(finalColor, 1.0);
}

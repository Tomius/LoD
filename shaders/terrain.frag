#version 330

in vec3 normal;
in vec3 camPos;
in vec3 worldPos;
in vec2 texCoord;
in float invalid;

uniform sampler2D GrassMap, GrassNormalMap;
uniform vec3 Scales;

out vec3 fragColor;

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();

float fogMin = max(Scales.x, Scales.z) * 256.0;
float fogMax = max(Scales.x, Scales.z) * 2048.0;
vec3 fogColor = vec3(0.6);
const float SpecularShininess = 5.0f;

void main() {
    if(invalid != 0.0)
        discard;

    vec2 grassTexCoord = texCoord * 1000;
    mat3 NormalMatrix;
    vec3 n_normal = normalize(normal);
    NormalMatrix[0] = cross(vec3(0.0, 0.0, 1.0), n_normal); // tangent - approximately (1, 0, 0)
    NormalMatrix[1] = cross(n_normal, NormalMatrix[0]); // bitangent - approximately (0, 0, 1)
    NormalMatrix[2] = normalize(normal); // normal - approximately (0, 1, 0)
    vec3 normalOffset = texture(GrassNormalMap, grassTexCoord).rgb;

    vec3 finalNormal = NormalMatrix * normalOffset;

    float d =  dot(
        finalNormal,
        normalize(AmbientDirection())
    );

    float DiffusePower = max(SunPower() * d, 0.0);

    vec3 grass = texture(GrassMap, grassTexCoord).rgb;

    vec3 Color = AmbientColor() * grass * (AmbientPower() + DiffusePower);
    vec3 Fog = fogColor * SunPower();

    float l = length(camPos);
    float alpha = clamp(
        (l - fogMin) / (fogMax - fogMin),
        0.0, 1.0
    ) / 4;

    fragColor = mix(Color, Fog, alpha);

}


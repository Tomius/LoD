#version 330

in vec3 normal;
in vec3 camPos;
in vec3 worldPos;
in vec2 texCoord;
in float invalid;

uniform sampler2D GrassMap[2], GrassNormalMap;
uniform vec3 Scales = vec3(1.0, 1.0, 1.0);

out vec3 fragColor;

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();

float fogMin = max(Scales.x, Scales.z) * 128.0;
float fogMax = max(Scales.x, Scales.z) * 2048.0;
vec3 fogColor = vec3(0.4);

const float specular_shininess = 0.2f;

void main() {
    if(invalid != 0.0)
        discard;

    vec2 grassTexCoord = texCoord * 400;
    mat3 NormalMatrix;
    vec3 n_normal = normalize(normal);
    NormalMatrix[0] = cross(vec3(0.0, 0.0, 1.0), n_normal); // tangent - approximately (1, 0, 0)
    NormalMatrix[1] = cross(n_normal, NormalMatrix[0]); // bitangent - approximately (0, 0, 1)
    NormalMatrix[2] = normalize(normal); // normal - approximately (0, 1, 0)
    vec3 normalOffset = texture(GrassNormalMap, grassTexCoord).rgb;

    vec3 finalNormal = NormalMatrix * normalOffset;

    vec3 lightDir = normalize(AmbientDirection());

    float cosAngIncidence = max(dot(finalNormal, lightDir), 0);
    vec3 viewDirection = normalize(-worldPos);

    vec3 halfAngle = normalize(lightDir + viewDirection);
    float angleNormalHalf = acos(dot(halfAngle, finalNormal));
    float exponent = angleNormalHalf / specular_shininess;
    exponent = -(exponent * exponent);
    float gaussianTerm = exp(exponent);
    if(cosAngIncidence == 0)
        gaussianTerm = 0;

    vec3 grass_0 = texture(GrassMap[0], grassTexCoord).rgb;
    vec3 grass_1 = texture(GrassMap[1], grassTexCoord).rgb;
    float height_factor = clamp(sqrt((worldPos.y - 15 * Scales.y) / 40.0f), 0.0f, 1.0f);
    vec3 grass = mix(grass_0, grass_1, height_factor);

    vec3 Color = AmbientColor() * grass * (SunPower() * (gaussianTerm + cosAngIncidence) + AmbientPower());
    vec3 Fog = fogColor * (0.05 + 0.95 * SunPower());

    float l = length(camPos);
    float alpha = clamp(
        (l - fogMin) / (fogMax - fogMin),
        0.0, 1.0
    ) / 4;

    fragColor = mix(Color, Fog, alpha);
}


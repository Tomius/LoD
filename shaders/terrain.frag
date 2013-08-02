#version 330

in vec3 normal;
in vec3 camPos;
in vec3 worldPos;
in vec2 texCoord;
uniform sampler2D ColorMap;
uniform vec3 Scales;

out vec3 fragColor;

vec3 AmbientDirection();
float SunPower();
float AmbientPower();

float fogMin = max(Scales.x, Scales.z) * 256.0;
float fogMax = max(Scales.x, Scales.z) * 2048.0;
vec3 fogColor = vec3(0.6);
const float SpecularShininess = 50.0f;

void main() {
    float d =  dot(
        normalize(normal),
        normalize(AmbientDirection())
    );
    float DiffusePower = max(SunPower() * d, 0.0);

    vec3 refl = reflect(AmbientDirection(), -normal);
    float SpecularFactor = max(
       dot(normalize(refl),
           normalize(vec3(worldPos.x, -worldPos.y, worldPos.z))
       ), 0.0
   );
    float SpecularPower = SunPower() * pow(SpecularFactor, SpecularShininess);

    vec3 texColor = texture(ColorMap, texCoord).rgb;
    vec3 Color = texColor * (AmbientPower() + DiffusePower + SpecularPower);
    vec3 Fog = fogColor * SunPower();

    float l = length(camPos);
    float alpha = clamp(
        (l - fogMin) / (fogMax - fogMin),
        0.0, 1.0
    ) / 2;

    fragColor = /*vec3(0.0, 1.0, 0.0) - 0.1 * */mix(Color, Fog, alpha);

}


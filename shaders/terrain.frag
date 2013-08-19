#version 330

in vec3 normal;
in vec3 camPos;
in vec3 worldPos;
in vec2 texCoord;
in float invalid;
uniform sampler2D ColorMap;
uniform vec3 Scales;

out vec3 fragColor;

// -------======{[ Lighting ]}======-------

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();

float fogMin = max(Scales.x, Scales.z) * 256.0;
float fogMax = max(Scales.x, Scales.z) * 2048.0;
vec3 fogColor = vec3(0.6);
const float SpecularShininess = 10.0f;

void main() {
    if(invalid != 0.0)
        discard;
    float d =  dot(
        normalize(normal),
        normalize(AmbientDirection())
    );
    float DiffusePower = max(SunPower() * d, 0.01);

    vec3 refl = reflect(AmbientDirection(), normalize(normal));
    float SpecularFactor = max(
       dot(normalize(refl),
           normalize(worldPos)
       ), 0.0
   );
    float SpecularPower = SunPower() * pow(SpecularFactor, SpecularShininess) / 2;

    vec3 texColor = texture(ColorMap, texCoord).rgb;
    vec3 Color = AmbientColor() * texColor * (AmbientPower() + DiffusePower + SpecularPower);
    vec3 Fog = fogColor * SunPower();

    float l = length(camPos);
    float alpha = clamp(
        (l - fogMin) / (fogMax - fogMin),
        0.0, 1.0
    ) / 4;

    fragColor = mix(Color, Fog, alpha);

}


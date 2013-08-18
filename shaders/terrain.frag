#version 330

in vec3 normal;
in vec3 camPos;
in vec3 worldPos;
in vec2 texCoord;
in vec4 shadowCoord;
in float invalid;
uniform sampler2D ColorMap;
uniform sampler2DShadow ShadowMap;
uniform vec3 Scales;

out vec3 fragColor;

// -------======{[ Shadow ]}======-------

// This should be between 1 and 16. The higher this value,
// the softer shadow it results
const int shadowSoftness = 16;

// The maximum potion of light that should be subtracted
// if the object is in shadow. For ex. 0.8 means, object in
// shadow is 20% as bright as a lit one.
const float maxShadow = 0.8;

// Random numbers with nicer properties than true random.
// Google "Poisson Disk Sampling".
vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

float Visibility() {
    float visibility = 1.0;
    float bias = 0.01;
    float alpha = maxShadow / shadowSoftness; // Max shadow per sample

	// Sample the shadow map shadowSoftness times.
	for(int i = 0; i < shadowSoftness; i++) {
		visibility -= alpha * (1.0 - texture(
                ShadowMap,
                vec3(
                     shadowCoord.xy + poissonDisk[i] / 1024.0,
                     (shadowCoord.z - bias) / shadowCoord.w)
                )
        );
	}

	return visibility;
}

// -------======{[ Lighting ]}======-------

vec3 AmbientDirection();
float SunPower();
float AmbientPower();

float fogMin = max(Scales.x, Scales.z) * 256.0;
float fogMax = max(Scales.x, Scales.z) * 2048.0;
vec3 fogColor = vec3(0.6);
const float SpecularShininess = 50.0f;

void main() {
    if(invalid != 0.0)
        discard;
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
    float SpecularPower = SunPower() * pow(SpecularFactor, SpecularShininess) / 2;

    vec3 texColor = texture(ColorMap, texCoord).rgb;
    vec3 Color = texColor * (AmbientPower() + Visibility() * (DiffusePower + SpecularPower));
    vec3 Fog = pow(Visibility(), 0.15) * fogColor * SunPower();

    float l = length(camPos);
    float alpha = clamp(
        (l - fogMin) / (fogMax - fogMin),
        0.0, 1.0
    ) / 2;

    fragColor = mix(Color, Fog, alpha);

}


#version 330 core

#define SHADOW_MAP_NUM 64

in VertexData {
    vec3 c_normal;
    vec3 w_pos, c_pos;
    vec2 texCoord;
} vin;

uniform sampler2D uDiffuseTexture, uSpecularTexture;
uniform sampler2DArrayShadow uShadowMap;
uniform mat4 uCameraMatrix;

uniform mat4 uShadowCP[SHADOW_MAP_NUM];
uniform int  uNumUsedShadowMaps;

out vec4 frag_color;

vec3 AmbientDirection();
float AmbientPower();
float SunPower();
vec3 AmbientColor();

const float kSpecularShininess = 20.0f;

// -------======{[ Shadow ]}======-------

// Multi-sampling rate for the shadows. This should be between 1 and 16.
// The higher this value is, the softer shadow it results.
const int kShadowSoftness = 16;

// The maximum potion of light that should be subtracted
// if the object is in shadow. For ex. 0.8 means, object in
// shadow is 20% as bright as a lit one.
const float kMaxShadow = 0.7;

// Random numbers with nicer properties than true random.
// Google "Poisson Disk Sampling".
vec2 kPoissonDisk[16] = vec2[](
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

bool isValidShadowCoord(vec4 s) {
  if(s.w == 0)
    return false;
  else {
    vec3 p = s.xyz / s.w;
    if(abs(p.x) > 1 || abs(p.y) > 1 || abs(p.z) > 1)
      return false;
  }
  return true;
}

float Visibility() {
  float visibility = 1.0;
  float bias = 0.01;

  // For every shadow casters
  for(int i = 0; i < max(uNumUsedShadowMaps, SHADOW_MAP_NUM); ++i) {
    vec4 shadowCoord = uShadowCP[i] * vec4(vin.w_pos, 1.0);
    // Check for shadowCoord validity.
    if(!isValidShadowCoord(shadowCoord))
      break;

    int shadow_softness = (i == 0) ? kShadowSoftness : 1; // Only self-shadow needs MSA.
    float alpha = kMaxShadow / shadow_softness; // Max shadow per sample

    // Sample the shadow map kShadowSoftness times.
    for(int j = 0; j < shadow_softness; ++j) {
      visibility -= alpha * (1.0 - texture(
        uShadowMap,
        vec4( // x, y, slice, depth
          shadowCoord.xy + kPoissonDisk[j] / 256.0,
          i, (shadowCoord.z - bias) / shadowCoord.w
        )
      ));
    }
  }

  // The visibility shouldn't go negative.
	return max(visibility, 0.0);
}

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
            c_viewDir
          ), 0.0
        ), kSpecularShininess
      );
    }

    vec3 color = texture(uDiffuseTexture, vin.texCoord).rgb;
    float spec_mask = texture(uSpecularTexture, vin.texCoord).r;

    vec3 final_color = color * AmbientColor() *
        (Visibility() * SunPower() * (diffuse_power + spec_mask * specular_power) + AmbientPower());

    frag_color = vec4(final_color, 1.0);
}

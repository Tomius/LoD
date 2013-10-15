#version 330 core

in VertexData {
  vec3  w_normal;
  vec3  c_pos, w_pos;
  vec2  texcoord;
  vec4  shadowCoord;
  float invalid;
  mat3  NormalMatrix;
} vin;

uniform mat4 uCameraMatrix;
uniform sampler2D uGrassMap[2], uGrassNormalMap;
uniform sampler2DShadow uShadowMap;
uniform vec3 uScales;

out vec4 frag_color;

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();

float kFogMin = max(uScales.x, uScales.z) * 128.0;
float kFogMax = max(uScales.x, uScales.z) * 2048.0;
const vec3 kFogColor = vec3(0.8f);

const float kSpecularShininess = 20.0;

// -------======{[ Shadow ]}======-------

// This should be between 1 and 16. The higher this value,
// the softer shadow it results
const int kShadowSoftness = 4;

// The maximum potion of light that should be subtracted
// if the object is in shadow. For ex. 0.8 means, object in
// shadow is 20% as bright as a lit one.
const float kMaxShadow = 0.8;

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

float Visibility() {
  float visibility = 1.0;
  float bias = 0.01;
  float alpha = kMaxShadow / kShadowSoftness; // Max shadow per sample

	// Sample the shadow map kShadowSoftness times.
	for(int i = 0; i < kShadowSoftness; i++) {
		visibility -= alpha * (1.0 - texture(
      uShadowMap,
      vec3(
        vin.shadowCoord.xy + kPoissonDisk[i] / 256.0,
        (vin.shadowCoord.z - bias) / vin.shadowCoord.w)
      )
    );
	}

	return visibility;
}

void main() {
    if(vin.invalid != 0.0) {
        discard;
    }

    // TexCoord
    vec2 grass_texcoord = vin.texcoord * 200;

    // Normals
    vec3 w_normal = normalize(vin.w_normal);
    vec3 normal_offset = texture(uGrassNormalMap, grass_texcoord).rgb;
    vec3 w_final_normal = normalize(vin.NormalMatrix * normal_offset);
    vec3 c_normal = (uCameraMatrix * vec4(w_final_normal, 0.0)).xyz;

    // Lighting directions.
    vec3 c_light_dir = normalize((uCameraMatrix * vec4(AmbientDirection(), 0)).xyz);
    vec3 c_view_direction = normalize(-(uCameraMatrix * vec4(vin.w_pos, 1)).xyz);

    // Lighting values.
    float diffuse_power = max(dot(c_normal, c_light_dir), 0);
    float specular_power;
    if(diffuse_power < 0.0) {
        specular_power = 0.0;
    } else {
        specular_power = pow(
            max(0.0,
                dot(
                    reflect(-c_light_dir, c_normal),
                    c_view_direction)
                ),
            kSpecularShininess
        );
    }

    // Colors.
    vec3 grass0_color = texture(uGrassMap[0], grass_texcoord).rgb;
    vec3 grass1_color = texture(uGrassMap[1], grass_texcoord).rgb;
    float height_factor = clamp(sqrt((vin.w_pos.y - 15 * uScales.y) / 40.0f), 0.0f, 1.0f);
    vec3 grass_color = mix(grass0_color, grass1_color, height_factor);

    float length_from_camera = length(vin.c_pos);

    vec3 final_color;
    if(length_from_camera < 50) {
      final_color = grass_color * AmbientColor() *
          (Visibility() * SunPower() * (specular_power + diffuse_power) + AmbientPower());
    } else {
      final_color = grass_color * AmbientColor() *
          (SunPower() * (specular_power + diffuse_power) + AmbientPower());
    }


    // Fog
    vec3 fog = AmbientColor() * kFogColor * (0.005 + SunPower());
    float alpha = clamp((length_from_camera - kFogMin) / (kFogMax - kFogMin), 0.0, 1.0) / 12;

    frag_color = vec4(mix(final_color, fog, alpha), 1.0);
}


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

// The maximum potion of light that should be subtracted
// if the object is in shadow. For ex. 0.8 means, object in
// shadow is 20% as bright as a lit one.
const float kMaxShadow = 0.8;

float Visibility() {
  float bias = 0.01;

	return kMaxShadow * texture(
    uShadowMap,
    vec3(vin.shadowCoord.xy, (vin.shadowCoord.z - bias) / vin.shadowCoord.w)
  );
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


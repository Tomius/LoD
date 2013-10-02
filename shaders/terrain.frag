#version 330 core

in VertexData {
  vec3  w_normal;
  vec3  c_pos, w_pos;
  vec2  m_texcoord;
  float invalid;
  mat3  NormalMatrix;
} vin;

uniform mat4 uCameraMatrix;
uniform sampler2D uGrassMap[2], uGrassNormalMap;
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

void main() {
    if(vin.invalid != 0.0) {
        discard;
    }

    // TexCoord
    vec2 grass_texcoord = vin.m_texcoord * 200;

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

    vec3 final_color =
        AmbientColor() * grass_color * (SunPower() * (specular_power + diffuse_power) + AmbientPower());

    // Fog
    vec3 fog = AmbientColor() * kFogColor * (0.005 + SunPower());
    float alpha = clamp((length(vin.c_pos) - kFogMin) / (kFogMax - kFogMin), 0.0, 1.0) / 12;

    frag_color = vec4(mix(final_color, fog, alpha), 1.0);
}


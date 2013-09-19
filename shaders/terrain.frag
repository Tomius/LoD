#version 330

in vec3 normal;
in vec3 camPos;
in vec3 worldPos;
in vec2 texCoord;
in float invalid;
in mat3 normalMatrix;

uniform mat4 CameraMatrix;
uniform sampler2D GrassMap[2], GrassNormalMap;
uniform vec3 Scales = vec3(1.0, 1.0, 1.0);

out vec4 fragColor;

vec3 AmbientDirection();
float SunPower();
float AmbientPower();
vec3 AmbientColor();

float fogMin = max(Scales.x, Scales.z) * 128.0;
float fogMax = max(Scales.x, Scales.z) * 2048.0;
vec3 fogColor = vec3(0.8f);

const float specular_shininess = 20.0;

void main() {
    if(invalid != 0.0) {
        discard;
    }

    // TexCoord
    vec2 grassTexCoord = texCoord * 200;

    // Normals
    vec3 n_normal = normalize(normal);
    vec3 normalOffset = texture(GrassNormalMap, grassTexCoord).rgb;
    vec3 finalNormal = normalize(normalMatrix * normalOffset);
    vec3 cam_normal = (CameraMatrix * vec4(finalNormal, 0.0)).xyz;

    // Lighting directions.
    vec3 lightDir = normalize((CameraMatrix * vec4(AmbientDirection(), 0)).xyz);
    vec3 viewDirection = normalize(-(CameraMatrix * vec4(worldPos, 1)).xyz);

    // Lighting values.
    float diffuse_power = max(dot(cam_normal, lightDir), 0);
    float specular_power;
    if(diffuse_power < 0.0) {
        specular_power = 0.0;
    } else {
        specular_power = pow(
            max(0.0,
                dot(
                    reflect(-lightDir, cam_normal),
                    viewDirection)
                ),
            specular_shininess
        );
    }

    // Colors.
    vec3 grass_0 = texture(GrassMap[0], grassTexCoord).rgb;
    vec3 grass_1 = texture(GrassMap[1], grassTexCoord).rgb;
    float height_factor = clamp(sqrt((worldPos.y - 15 * Scales.y) / 40.0f), 0.0f, 1.0f);
    vec3 grass = mix(grass_0, grass_1, height_factor);

    vec3 Color = AmbientColor() * grass * (SunPower() * (specular_power + diffuse_power) + AmbientPower());

    // Fog
    vec3 Fog = AmbientColor() * fogColor * (0.005 + SunPower());
    float l = length(camPos);
    float alpha = clamp((l - fogMin) / (fogMax - fogMin), 0.0, 1.0) / 12;

    fragColor = vec4(mix(Color, Fog, alpha), 1.0);
}


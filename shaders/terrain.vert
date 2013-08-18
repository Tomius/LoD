#version 330

layout(location = 0) in vec2 Position;
uniform mat4 ProjectionMatrix, CameraMatrix;
uniform vec2 Offset;
uniform vec3 Scales;
uniform mat4 ShadowMVP;
uniform sampler2D HeightMap, NormalMap;

out vec3 normal;
out vec3 camPos;
out vec3 worldPos;
out vec2 texCoord;
out vec4 shadowCoord;
out float invalid;

void main() {
    vec2 offsPos = Position + Offset;
    ivec2 texSize = textureSize(HeightMap, 0);
    // The texture's center is at the world space origin.
    // Warning: the texCoord calculation is VERY inaccurate (f*ck floats)
    texCoord = (ivec2(round(offsPos)) + texSize / 2) / vec2(texSize);
    if(abs(offsPos.x) >= (texSize.x - 1) / 2 || abs(offsPos.y) >= (texSize.x - 1) / 2)
        invalid = 1e10;
    else
        invalid = 0.0;

    float height = texture(HeightMap, texCoord).r * 255;
    worldPos = Scales * vec3(offsPos.x, height, offsPos.y);
    camPos = (CameraMatrix * vec4(worldPos, 1.0)).xyz;
    shadowCoord = ShadowMVP * vec4(worldPos, 1.0);

    normal = texture(NormalMap, texCoord).rgb;

    gl_Position = ProjectionMatrix * vec4(camPos, 1.0);
}

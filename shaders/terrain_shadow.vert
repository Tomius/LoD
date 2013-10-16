#version 330 core

// DEPRECATED!! DON'T USE

layout(location = 0) in vec2 Position;
uniform mat4 MVP;
uniform vec2 Offset;
uniform vec3 Scales;
uniform sampler2D HeightMap;

void main() {
    vec2 offsPos = Position + Offset;
    ivec2 texSize = textureSize(HeightMap, 0);
    vec2 texCoord = (ivec2(round(offsPos)) + texSize / 2) / vec2(texSize);

    float height = texture(HeightMap, texCoord).r * 255;
    vec3 Pos = Scales * vec3(offsPos.x, height, offsPos.y);

    gl_Position = MVP * vec4(Pos, 1.0);
}

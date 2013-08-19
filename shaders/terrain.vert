#version 330

layout(location = 0) in ivec2 Position;
uniform mat4 ProjectionMatrix, CameraMatrix;
uniform ivec2 Offset;
uniform vec3 Scales;
uniform sampler2D HeightMap;

out vec3 normal;
out vec3 camPos;
out vec3 worldPos;
out vec2 texCoord;
out float invalid;

float Height(ivec2 texCoord) {
    return texelFetch(HeightMap, texCoord, 0).r * 255;
}

void main() {
    ivec2 offsPos = (Position + Offset) / 2;
    ivec2 texSize = textureSize(HeightMap, 0);
    if(abs(offsPos.x) >= (texSize.x - 1) / 2 || abs(offsPos.y) >= (texSize.x - 1) / 2)
        invalid = 1e10;
    else
        invalid = 0.0;

    ivec2 iTexCoord = (ivec2(offsPos) + texSize / 2);
    texCoord = iTexCoord / vec2(texSize);

    float height = Height(iTexCoord);
    worldPos = Scales * vec3(offsPos.x, height, offsPos.y);
    camPos = (CameraMatrix * vec4(worldPos, 1.0)).xyz;

    // -------======{[ Normals ]}======-------

    // Horizontal slope
    float sHoriz = Height(iTexCoord + ivec2(1, 0)) - Height(iTexCoord + ivec2(-1, 0));
    // Slope along the main diagonal.
    float sMainD = Height(iTexCoord + ivec2(1, -1)) - Height(iTexCoord + ivec2(-1, 1));
    // Slope along the minor diagonal.
    float sMinorD = Height(iTexCoord + ivec2(-1, -1)) - Height(iTexCoord + ivec2(1, 1));

    normal = vec3(
        (sHoriz + sMainD - sMinorD) * Scales.y,
        // it should be multiplied by 3, but this way,
        // it is less visible that my approximation actually sux.
        12 * Scales.x,
        (sMinorD + sMainD) * Scales.y
    );

    gl_Position = ProjectionMatrix * vec4(camPos, 1.0);
}

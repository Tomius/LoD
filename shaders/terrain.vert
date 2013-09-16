#version 330

layout(location = 0) in ivec2 Position;
uniform mat4 ProjectionMatrix, CameraMatrix;
uniform ivec2 Offset;
uniform vec3 Scales;
uniform sampler2D HeightMap;
uniform int MipmapLevel;

out vec3 normal;
out vec3 camPos;
out vec3 worldPos;
out vec2 texCoord;
out float invalid;
out mat3 normalMatrix;

float Height(ivec2 texCoord) {
    return texelFetch(HeightMap, texCoord, 0).r * 255;
}

void main() {
    ivec2 iOffsPos = (Position + Offset) / 2;
    vec2 offsPos = (Position + Offset) / 2.0;
    ivec2 texSize = textureSize(HeightMap, 0);
    if(abs(int(offsPos.x)) - 1 >= texSize.x / 2 || abs(int(offsPos.y)) - 1 >= texSize.x / 2)
        invalid = 1e10;
    else
        invalid = 0.0;

    ivec2 iTexCoord = ivec2(offsPos) + texSize / 2;
    texCoord = iTexCoord / vec2(texSize);

    float height = Height(iTexCoord);
    worldPos = Scales * vec3(offsPos.x, height, offsPos.y);
    camPos = (CameraMatrix * vec4(worldPos, 1.0)).xyz;

    // -------======{[ Normals ]}======-------

    const ivec2 iNeighbours[6] = ivec2[6](
        ivec2(1, 2),   ivec2(2, 0),  ivec2(1, -2),
        ivec2(-1, -2), ivec2(-2, 0), ivec2(-1, 2)
    );

    vec3 neighbours[6];
    for(int i = 0; i < 6; i++) {
        ivec2 nPos = (Position + Offset + ((1 << (MipmapLevel + 1)) * iNeighbours[i])) / 2;
        ivec2 nTexCoord = nPos + texSize / 2;
        neighbours[i] = Scales * vec3(nPos.x, Height(nTexCoord), nPos.y) - worldPos;
    }

    vec3 tempNormal = vec3(0.0);
    for(int i = 0; i < 6; i++) {
        tempNormal += normalize(cross(neighbours[i], neighbours[(i+1) % 6]));
    }

    normal = normalize(tempNormal);

    normalMatrix[0] = cross(vec3(0.0, 0.0, 1.0), normal); // tangent - approximately (1, 0, 0)
    normalMatrix[1] = cross(normal, normalMatrix[0]); // bitangent - approximately (0, 0, 1)
    normalMatrix[2] = normal; // normal - approximately (0, 1, 0)

    gl_Position = ProjectionMatrix * vec4(camPos, 1.0);
}

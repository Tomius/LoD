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

float Height(ivec2 texCoord) {
    return texelFetch(HeightMap, texCoord, 0).r * 255;
}

void main() {
    ivec2 offsPos = (Position + Offset) / 2;
    ivec2 texSize = textureSize(HeightMap, 0);
    if(abs(offsPos.x) - 1 >= texSize.x / 2 || abs(offsPos.y) - 1 >= texSize.x / 2)
        invalid = 1e10;
    else
        invalid = 0.0;

    ivec2 iTexCoord = offsPos + texSize / 2;
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

    normal = tempNormal;


    gl_Position = ProjectionMatrix * vec4(camPos, 1.0);
}

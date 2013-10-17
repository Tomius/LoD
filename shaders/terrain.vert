#version 330 core

// Actually this number might be modified in the C++
// program's runtime, but before shader compilation.
#define SHADOW_MAP_NUM 4

layout(location = 0) in ivec2 vPosition;

uniform mat4 uProjectionMatrix, uCameraMatrix;
uniform ivec2 uOffset;
uniform vec3 uScales;
uniform sampler2D uHeightMap;
uniform int uMipmapLevel;

uniform mat4 uShadowCP[SHADOW_MAP_NUM];
uniform int  uNumUsedShadowMaps;

out VertexData {
  vec3  w_normal;
  vec3  c_pos, w_pos;
  vec2  texcoord;
  vec4  shadowCoord[SHADOW_MAP_NUM];
  float scInvalid[SHADOW_MAP_NUM];
  float invalid;
  mat3  NormalMatrix;
} vout;


float fetchHeight(ivec2 texCoord) {
    return texelFetch(uHeightMap, texCoord, 0).r * 255;
}

void main() {
    vec2 w_offsPos = (vPosition + uOffset) / 2.0;

    ivec2 tex_size = textureSize(uHeightMap, 0);
    if(abs(int(w_offsPos.x)) - 1 >= tex_size.x / 2 || abs(int(w_offsPos.y)) - 1 >= tex_size.y / 2) {
        vout.invalid = 1e10;
        gl_Position = vec4(0.0);
        return;
    } else {
        vout.invalid = 0.0;
    }

    ivec2 iTexcoord = ivec2(w_offsPos) + tex_size / 2;
    vout.texcoord = iTexcoord / vec2(tex_size);

    float height = fetchHeight(iTexcoord);
    vout.w_pos = uScales * vec3(w_offsPos.x, height, w_offsPos.y);
    vout.c_pos = (uCameraMatrix * vec4(vout.w_pos, 1.0)).xyz;
    vec4 w_pos = vec4(vout.w_pos, 1.0);

    // The performance drop is because of this loop.
    for(int i = 0; i < min(uNumUsedShadowMaps, SHADOW_MAP_NUM); ++i) {
        vec4 sc = uShadowCP[i] * w_pos;
        vout.shadowCoord[i] = sc;
        if(sc.w == 0) {
            vout.scInvalid[i] = 1e10;
        } else {
            sc.xyz /= sc.w;
            if(abs(sc.x) > 1.0 || abs(sc.y) > 1.0 || abs(sc.z) > 1.0) {
                vout.scInvalid[i] = 1e10;
            } else {
                vout.scInvalid[i] = 0.0;
            }
        }
    }

    // -------======{[ Normals ]}======-------

    const ivec2 iNeighbours[6] = ivec2[6](
        ivec2(1, 2),   ivec2(2, 0),  ivec2(1, -2),
        ivec2(-1, -2), ivec2(-2, 0), ivec2(-1, 2)
    );

    vec3 neighbours[6];
    for(int i = 0; i < 6; i++) {
        ivec2 nPos = (vPosition + uOffset + ((1 << (uMipmapLevel + 1)) * iNeighbours[i])) / 2;
        ivec2 nTexcoord = nPos + tex_size / 2;
        neighbours[i] = uScales * vec3(nPos.x, fetchHeight(nTexcoord), nPos.y) - vout.w_pos;
    }

    vec3 temp_normal = vec3(0.0);
    for(int i = 0; i < 6; i++) {
        temp_normal += normalize(cross(neighbours[i], neighbours[(i+1) % 6]));
    }

    vout.w_normal = normalize(temp_normal);

    vout.NormalMatrix[0] = cross(vec3(0.0, 0.0, 1.0), vout.w_normal); // tangent - approximately (1, 0, 0)
    vout.NormalMatrix[1] = cross(vout.w_normal, vout.NormalMatrix[0]); // bitangent - approximately (0, 0, 1)
    vout.NormalMatrix[2] = vout.w_normal; // normal - approximately (0, 1, 0)

    gl_Position = uProjectionMatrix * vec4(vout.c_pos, 1.0);
}

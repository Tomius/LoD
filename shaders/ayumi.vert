#version 330 core

// External macros
#define BONE_NUM
#define BONE_ATTRIB_NUM
#define SHADOW_MAP_NUM 4

// If you reorder or change the layout of these,
// remember to that to ayumi_shadow.vert too!
in vec4  vPosition;
in ivec4 vBoneIDs[BONE_ATTRIB_NUM];
in vec4  vWeights[BONE_ATTRIB_NUM];

in vec2 vTexCoord;
in vec3 vNormal;

uniform mat4 uShadowCP[SHADOW_MAP_NUM];
uniform int  uNumUsedShadowMaps;

uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;
uniform mat4 uBones[BONE_NUM];

out VertexData {
    vec3 c_normal;
    vec3 c_pos;
    vec2 texCoord;
    vec4 shadowCoord[SHADOW_MAP_NUM];
    float scInvalid[SHADOW_MAP_NUM];
} vout;

void main() {
    mat4 BoneMatrix = mat4(0);
    for(int i = 0; i < BONE_ATTRIB_NUM; i++)
        for(int j = 0; j < 4; j++)
            BoneMatrix += uBones[vBoneIDs[i][j]] * vWeights[i][j];

    vout.c_normal = vec3(uCameraMatrix * (uModelMatrix * (BoneMatrix * vec4(vNormal, 0.0))));

    vout.texCoord = vTexCoord;

    vec4 w_pos = uModelMatrix * (BoneMatrix * vPosition);
    vec4 c_pos = uCameraMatrix * w_pos;
    vout.c_pos = vec3(c_pos);

    for(int i = 0; i < max(uNumUsedShadowMaps, SHADOW_MAP_NUM); ++i) {
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

    gl_Position = uProjectionMatrix * c_pos;
}

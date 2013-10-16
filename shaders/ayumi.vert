#version 330 core

// External macros
#define BONE_NUM
#define BONE_ATTRIB_NUM

// If you reorder or change the layout of these,
// remember to that to ayumi_shadow.vert too!
in vec4  vPosition;
in ivec4 vBoneIDs[BONE_ATTRIB_NUM];
in vec4  vWeights[BONE_ATTRIB_NUM];

in vec2 vTexCoord;
in vec3 vNormal;

uniform mat4 uShadowCP, uProjectionMatrix, uCameraMatrix, uModelMatrix;
uniform mat4 uBones[BONE_NUM];

out VertexData {
    vec3 c_normal;
    vec3 c_pos;
    vec2 texCoord;
    vec4 shadowCoord;
} vout;

void main() {
    mat4 BoneMatrix = mat4(0);
    for(int i = 0; i < BONE_ATTRIB_NUM; i++)
        for(int j = 0; j < 4; j++)
            BoneMatrix += uBones[vBoneIDs[i][j]] * vWeights[i][j];

    vout.c_normal = vec3(uCameraMatrix * (uModelMatrix * (BoneMatrix * vec4(vNormal, 0.0))));
    vout.shadowCoord = uShadowCP * (uModelMatrix * (BoneMatrix * vPosition));
    vout.texCoord = vTexCoord;

    vec4 c_pos = uCameraMatrix * (uModelMatrix * (BoneMatrix * vPosition));
    vout.c_pos = vec3(c_pos);

    gl_Position = uProjectionMatrix * c_pos;
}

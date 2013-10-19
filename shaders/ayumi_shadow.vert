#version 330 core

// External macros
#define BONE_NUM
#define BONE_ATTRIB_NUM

in vec4  vPosition;
in ivec4 vBoneIDs[BONE_ATTRIB_NUM];
in vec4  vWeights[BONE_ATTRIB_NUM];

uniform mat4 uMCP;
uniform mat4 uBones[BONE_NUM];

void main() {
  mat4 BoneMatrix = mat4(0);
  for(int i = 0; i < BONE_ATTRIB_NUM; i++)
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[vBoneIDs[i][j]] * vWeights[i][j];

  gl_Position = uMCP * (BoneMatrix * vPosition);
}

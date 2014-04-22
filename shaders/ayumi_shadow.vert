#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

// External macros
#define BONE_NUM
#define BONE_ATTRIB_NUM

attribute vec4 aPosition;

#if BONE_ATTRIB_NUM > 0
attribute vec4 aBoneIDs0;
attribute vec4 aWeights0;
#endif
#if BONE_ATTRIB_NUM > 1
attribute vec4 aBoneIDs1;
attribute vec4 aWeights1;
#endif
#if BONE_ATTRIB_NUM > 2
attribute vec4 aBoneIDs2;
attribute vec4 aWeights2;
#endif
#if BONE_ATTRIB_NUM > 3
attribute vec4 aBoneIDs3;
attribute vec4 aWeights3;
#endif
#if BONE_ATTRIB_NUM > 4
attribute vec4 aBoneIDs4;
attribute vec4 aWeights4;
#endif
#if BONE_ATTRIB_NUM > 5
attribute vec4 aBoneIDs5;
attribute vec4 aWeights5;
#endif
#if BONE_ATTRIB_NUM > 6
attribute vec4 aBoneIDs6;
attribute vec4 aWeights6;
#endif
#if BONE_ATTRIB_NUM > 7
attribute vec4 aBoneIDs7;
attribute vec4 aWeights7;
#endif

uniform mat4 uMCP;
uniform mat4 uBones[BONE_NUM];

mat4 getBoneMatrix() {
  mat4 BoneMatrix = mat4(0);
  #if BONE_ATTRIB_NUM > 0
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[int(aBoneIDs0[j])] * aWeights0[j];
  #endif
  #if BONE_ATTRIB_NUM > 1
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[int(aBoneIDs1[j])] * aWeights1[j];
  #endif
  #if BONE_ATTRIB_NUM > 2
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[int(aBoneIDs2[j])] * aWeights2[j];
  #endif
  #if BONE_ATTRIB_NUM > 3
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[int(aBoneIDs3[j])] * aWeights3[j];
  #endif
  #if BONE_ATTRIB_NUM > 4
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[int(aBoneIDs4[j])] * aWeights4[j];
  #endif
  #if BONE_ATTRIB_NUM > 5
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[int(aBoneIDs5[j])] * aWeights5[j];
  #endif
  #if BONE_ATTRIB_NUM > 6
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[int(aBoneIDs6[j])] * aWeights6[j];
  #endif
  return BoneMatrix;
}

void main() {
  gl_Position = uMCP * (getBoneMatrix() * aPosition);
}

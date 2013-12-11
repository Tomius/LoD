#version 150

// External macros
#define BONE_NUM
#define BONE_ATTRIB_NUM

// If you reorder or change the layout of these,
// remember to that to ayumi_shadow.vert too!
in vec4  aPosition;
in ivec4 aBoneIDs[BONE_ATTRIB_NUM];
in vec4  aWeights[BONE_ATTRIB_NUM];

in vec2 aTexCoord;
in vec3 aNormal;

uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;
uniform mat4 uBones[BONE_NUM];

out vec3 c_vNormal;
out vec3 w_vPos, c_vPos;
out vec2 vTexCoord;

void main() {
  mat4 BoneMatrix = mat4(0);
  for(int i = 0; i < BONE_ATTRIB_NUM; i++)
    for(int j = 0; j < 4; j++)
      BoneMatrix += uBones[aBoneIDs[i][j]] * aWeights[i][j];

  c_vNormal = vec3(uCameraMatrix * (uModelMatrix * (BoneMatrix * vec4(aNormal, 0.0))));
  vTexCoord = aTexCoord;

  vec4 w_pos = uModelMatrix * (BoneMatrix * aPosition);
  vec4 c_pos = uCameraMatrix * w_pos;

  c_vPos = vec3(c_pos);
  w_vPos = vec3(w_pos);

  gl_Position = uProjectionMatrix * c_pos;
}

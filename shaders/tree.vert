#version 150

in vec4 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;

uniform mat4 uModelCameraMatrix, uProjectionMatrix;
uniform mat3 uNormalMatrix;

out vec3 c_vPos;
out vec3 w_vNormal;
out vec2 vTexcoord;

void main() {
  w_vNormal = aNormal * uNormalMatrix;
  vTexcoord = aTexCoord;

  vec4 c_pos = uModelCameraMatrix * aPosition;
  c_vPos = vec3(c_pos);

  gl_Position = uProjectionMatrix * c_pos;
}

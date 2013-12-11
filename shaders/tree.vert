#version 150

in vec4 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;

uniform mat4 uModelCamera, uProjection;
uniform mat3 uInvTranspModel;

out vec3 c_vPos;
out vec3 w_vNormal;
out vec2 vTexcoord;

void main() {
  w_vNormal = uInvTranspModel * aNormal;
  vTexcoord = aTexCoord;

  vec4 c_pos = uModelCamera * aPosition;
  c_vPos = vec3(c_pos);

  gl_Position = uProjection * c_pos;
}

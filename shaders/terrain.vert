#version 120

attribute vec2 aPositionFloat;
ivec2 aPosition = ivec2(aPositionFloat);

uniform mat4 uProjectionMatrix, uCameraMatrix;
uniform ivec2 uOffset, uTexSize;
uniform vec3 uScales;
uniform sampler2D uHeightMap;
uniform int uMipmapLevel;

varying vec3  w_vNormal;
varying vec3  c_vPos, w_vPos;
varying vec2  vTexcoord;
varying float vInvalid;
varying mat3  vNormalMatrix;

float fetchHeight(ivec2 texCoord) {
  return texture2D(uHeightMap, texCoord/vec2(uTexSize)).r * 255;
}

void main() {
  vec2 w_offsPos = (aPosition + uOffset) / 2.0;

  if(int(abs(w_offsPos.x)) - 1 >= uTexSize.x / 2 || int(abs(w_offsPos.y)) - 1 >= uTexSize.y / 2) {
    vInvalid = 1e10;
    gl_Position = vec4(0.0);
    return;
  } else {
    vInvalid = 0.0;
  }

  ivec2 iTexcoord = ivec2(w_offsPos) + uTexSize / 2;
  vTexcoord = iTexcoord / vec2(uTexSize);
  float height = fetchHeight(iTexcoord);

  vec4 w_pos = vec4(uScales * vec3(w_offsPos.x, height, w_offsPos.y), 1.0);
  vec4 c_pos = uCameraMatrix * w_pos;

  w_vPos = vec3(w_pos);
  c_vPos = vec3(c_pos);


  // -------======{[ Normals ]}======-------

  const ivec2 iNeighbours[6] = ivec2[6](
    ivec2(1, 2),   ivec2(2, 0),  ivec2(1, -2),
    ivec2(-1, -2), ivec2(-2, 0), ivec2(-1, 2)
  );

  vec3 neighbours[6];
  for(int i = 0; i < 6; i++) {
    int neighbourDistance = int(pow(2, (uMipmapLevel + 1))); // no << in #version 120
    ivec2 nPos = (aPosition + uOffset + (neighbourDistance * iNeighbours[i])) / 2;
    ivec2 nTexcoord = nPos + uTexSize / 2;
    neighbours[i] = uScales * vec3(nPos.x, fetchHeight(nTexcoord), nPos.y) - vec3(w_pos);
  }

  vec3 w_normal = vec3(0.0);

  // For loop is for noobs. And for those who have #version 130, and operator%.
  w_normal += normalize(cross(neighbours[0], neighbours[1]));
  w_normal += normalize(cross(neighbours[1], neighbours[2]));
  w_normal += normalize(cross(neighbours[2], neighbours[3]));
  w_normal += normalize(cross(neighbours[3], neighbours[4]));
  w_normal += normalize(cross(neighbours[4], neighbours[5]));
  w_normal += normalize(cross(neighbours[5], neighbours[0]));

  w_normal = normalize(w_normal);
  w_vNormal = w_normal;

  vec3 w_tangent = cross(vec3(0.0, 0.0, 1.0), w_normal);
  vec3 w_bitangent = cross(w_normal, w_tangent);

  vNormalMatrix[0] = w_tangent;
  vNormalMatrix[1] = w_bitangent;
  vNormalMatrix[2] = w_normal;

  gl_Position = uProjectionMatrix * c_pos;
}

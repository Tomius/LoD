#version 140

in ivec2 aPosition;

uniform mat4 uProjectionMatrix, uCameraMatrix;
uniform ivec2 uOffset;
uniform vec3 uScales;
uniform sampler2D uHeightMap;
uniform int uMipmapLevel;

out vec3  w_vNormal;
out vec3  c_vPos, w_vPos;
out vec2  vTexcoord;
out float vInvalid;
out mat3  vNormalMatrix;

float fetchHeight(ivec2 texCoord) {
  return texelFetch(uHeightMap, texCoord, 0).r * 255;
}

void main() {
  vec2 w_offsPos = (aPosition + uOffset) / 2.0;

  ivec2 tex_size = textureSize(uHeightMap, 0);
  if(abs(int(w_offsPos.x)) - 1 >= tex_size.x / 2 || abs(int(w_offsPos.y)) - 1 >= tex_size.y / 2) {
    vInvalid = 1e10;
    gl_Position = vec4(0.0);
    return;
  } else {
    vInvalid = 0.0;
  }

  ivec2 iTexcoord = ivec2(w_offsPos) + tex_size / 2;
  vTexcoord = iTexcoord / vec2(tex_size);
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
    ivec2 nPos = (aPosition + uOffset + ((1 << (uMipmapLevel + 1)) * iNeighbours[i])) / 2;
    ivec2 nTexcoord = nPos + tex_size / 2;
    neighbours[i] = uScales * vec3(nPos.x, fetchHeight(nTexcoord), nPos.y) - vec3(w_pos);
  }

  vec3 w_normal = vec3(0.0);
  for(int i = 0; i < 6; i++) {
    w_normal += normalize(cross(neighbours[i], neighbours[(i+1) % 6]));
  }

  w_normal = normalize(w_normal);
  w_vNormal = w_normal;

  vec3 w_tangent = cross(vec3(0.0, 0.0, 1.0), w_normal);
  vec3 w_bitangent = cross(w_normal, w_tangent);

  vNormalMatrix[0] = w_tangent;
  vNormalMatrix[1] = w_bitangent;
  vNormalMatrix[2] = w_normal;

  gl_Position = uProjectionMatrix * c_pos;
}

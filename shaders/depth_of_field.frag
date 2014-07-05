// Copyright (c) 2014, Tamas Csala

#version 120

#export vec3 DoF(vec3 texel_color);

uniform sampler2D uTex, uDepthTex;
uniform float uZNear, uZFar;
uniform vec2 uScreenSize;

vec2 coord = ivec2(gl_FragCoord.xy) / uScreenSize;
int mipmap_count = 1 + int(log2(max(uScreenSize.x, uScreenSize.y)));

// see http://web.archive.org/web/20130416194336/http://olivers.posterous.com/linear-depth-in-glsl-for-real
float DistanceFromCamera() {
  float z_b = texture2D(uDepthTex, coord).x;
  float z_n = 2.0 * z_b - 1.0;
  return 2.0 * uZNear * uZFar / (uZFar + uZNear - z_n * (uZFar - uZNear));
}

vec3 DoF(vec3 texel_color) {
  float level = sqrt(DistanceFromCamera() / uZFar) * (mipmap_count-1);
  float floor_level = floor(level);
  vec3 color = texel_color;
  for (int i = 1; i <= floor_level; ++i) {
    color += texture2DLod(uTex, coord, i).rgb;
  }
  color += (level-floor_level) * texture2DLod(uTex, coord, floor_level+1).rgb;
  return color / (1 + level);
}

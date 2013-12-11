#version 150

uniform sampler2DRect uTex;

out vec4 vFragColor;

void main() {
  vec3 color;
  vec3 sum = vec3(0.0);
  ivec2 texcoord = ivec2(gl_FragCoord.xy);

  const int n = 2; // max distance in which pixels also get fetched
  for(int dx = -n; dx <= n; dx++) {
    for(int dy = -n; dy <= n; dy++) {
      sum += pow(texelFetch(uTex, texcoord + (2 * ivec2(dx, dy))).rgb, vec3(3.0)) / (n*n);
    }
  }
  vec3 texel = texelFetch(uTex, texcoord).rgb;

  float lumiosity = length(texel) / sqrt(3.0);
  if(lumiosity < 0.3) {
    color = (sum * 0.09 + texel).rgb;
  } else {
    if(lumiosity < 0.5) {
      color = (sum * 0.06 + texel).rgb;
    } else {
      color = (sum * 0.04 + texel).rgb;
    }
  }

  // Apply a simple HDR
  lumiosity = length(color);
  color *= lumiosity / (lumiosity + 1);

  vFragColor = vec4(color, 1.0);
}

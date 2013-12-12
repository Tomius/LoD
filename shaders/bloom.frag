#version 150

uniform sampler2DRect uTex;

out vec4 vFragColor;

// Faster than pow(v, 3)
vec3 cube(vec3 v) {
  return v*v*v;
}

// This should be two nested for cycle if performance wasn't an issue...
vec3 filter(in ivec2 texcoord) {
  const float isq2 = 1.0/sqrt(2.0), isq3 = 1.0/sqrt(3.0);

  vec3 sum = vec3(0.0);
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(-1, -1)).rgb) * isq3;
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(-1, 0)).rgb) * isq2;
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(-1, 1)).rgb) * isq3;
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(0, -1)).rgb) * isq2;
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(0, 1)).rgb) * isq2;
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(1, -1)).rgb) * isq3;
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(1, 0)).rgb) * isq2;
  sum += cube(texelFetchOffset(uTex, texcoord, ivec2(1, 1)).rgb) * isq3;

  return sum;
}

void main() {
  ivec2 texcoord = ivec2(gl_FragCoord.xy);
  vec3 sum = filter(texcoord);

  vec3 texel = texelFetch(uTex, texcoord).rgb;

  float lumiosity = length(texel) / sqrt(3.0);
  vec3 color = sum * 0.5*(1.2-lumiosity) + texel.rgb;

  // Apply a simple HDR
  lumiosity = length(color);
  color *= lumiosity / (lumiosity + 1);

  vFragColor = vec4(color, 1.0);
}

#version 120

#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

uniform sampler2D uTex;
uniform vec2 uScreenSize;

// Faster than pow(v, 3)
vec3 cube(vec3 v) {
  return v*v*v;
}

vec3 fetch(in ivec2 texcoord) {
  return texture2D(uTex, texcoord/uScreenSize).rgb;
}

// This should be two nested for cycle if performance wasn't an issue...
vec3 filter(in ivec2 texcoord) {
  const float isq2 = 1.0/sqrt(2.0), isq3 = 1.0/sqrt(3.0);

  vec3 sum = vec3(0.0);
  sum += cube(fetch(texcoord + ivec2(-1, -1))) * isq3;
  sum += cube(fetch(texcoord + ivec2(-1, 0))) * isq2;
  sum += cube(fetch(texcoord + ivec2(-1, 1))) * isq3;
  sum += cube(fetch(texcoord + ivec2(0, -1))) * isq2;
  sum += cube(fetch(texcoord + ivec2(0, 1))) * isq2;
  sum += cube(fetch(texcoord + ivec2(1, -1))) * isq3;
  sum += cube(fetch(texcoord + ivec2(1, 0))) * isq2;
  sum += cube(fetch(texcoord + ivec2(1, 1))) * isq3;

  return sum;
}

void main() {
  ivec2 texcoord = ivec2(gl_FragCoord.xy);
  vec3 sum = filter(texcoord);

  vec3 texel = fetch(texcoord);

  float lumiosity = length(texel) / sqrt(3.0);
  vec3 color = pow(sum * 0.5*(1.2-lumiosity) + texel, vec3(1.4));

  gl_FragColor = clamp(vec4(color, 1.0), vec4(0.0), vec4(1.0));
}

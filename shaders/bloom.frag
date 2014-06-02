// Copyright (c) 2014, Tamas Csala

#version 120

uniform sampler2D uTex;
uniform vec2 uScreenSize;

// Faster than pow(v, 3)
vec3 cube(vec3 v) {
  return v*v*v;
}

vec3 fetch(in ivec2 tex_coord) {
  return texture2D(uTex, tex_coord/uScreenSize).rgb;
}

// This should be two nested for cycle if performance wasn't an issue...
vec3 filter(in ivec2 tex_coord) {
  const float isq2 = 1.0/sqrt(2.0), isq3 = 1.0/sqrt(3.0);

  vec3 sum = vec3(0.0);
  sum += cube(fetch(tex_coord + ivec2(-1, -1))) * isq3;
  sum += cube(fetch(tex_coord + ivec2(-1, 0))) * isq2;
  sum += cube(fetch(tex_coord + ivec2(-1, 1))) * isq3;
  sum += cube(fetch(tex_coord + ivec2(0, -1))) * isq2;
  sum += cube(fetch(tex_coord + ivec2(0, 1))) * isq2;
  sum += cube(fetch(tex_coord + ivec2(1, -1))) * isq3;
  sum += cube(fetch(tex_coord + ivec2(1, 0))) * isq2;
  sum += cube(fetch(tex_coord + ivec2(1, 1))) * isq3;

  return sum;
}

void main() {
  ivec2 tex_coord = ivec2(gl_FragCoord.xy);
  vec3 glow = filter(tex_coord);

  vec3 texel = fetch(tex_coord);

  float lumiosity = length(texel) / sqrt(3.0);
  vec3 color = glow*(1.2-lumiosity) + texel;

  // Filmic tone mapping approximation, by Jim Hejl
  // vec3 x = max(color - 0.004, 0.0);
  // color = (x*(6.2*x+0.5))/(x*(6.2*x+1.7)+0.06);

  gl_FragColor = clamp(vec4(color, 1.0), vec4(0.0), vec4(1.0));
}

// Copyright (c) 2014, Tamas Csala

#version 120

uniform sampler2D uTex;
uniform vec2 uScreenSize;

// Faster than pow(v, 3)
vec3 cube(vec3 v) {
  return v*v*v;
}

vec3 fetch(ivec2 tex_coord) {
  return texture2D(uTex, tex_coord/uScreenSize).rgb;
}

// This should be two nested for cycles if performance wasn't an issue...
vec3 filter(ivec2 tex_coord) {
  return cube(fetch(tex_coord + ivec2(-1, -1)));
       + cube(fetch(tex_coord + ivec2(-1,  0))) * 2;
       + cube(fetch(tex_coord + ivec2(-1, +1)));
       + cube(fetch(tex_coord + ivec2( 0, -1))) * 2;
       + cube(fetch(tex_coord + ivec2( 0,  0))) * 4;
       + cube(fetch(tex_coord + ivec2( 0, +1))) * 2;
       + cube(fetch(tex_coord + ivec2(+1, -1)));
       + cube(fetch(tex_coord + ivec2(+1,  0))) * 2;
       + cube(fetch(tex_coord + ivec2(+1, +1)));
}

vec3 bloom_fetch(ivec2 tex_coord) {
  vec3 texel_color = fetch(tex_coord);
  vec3 glow = filter(tex_coord);
  return glow + texel_color;
}

vec3 filmic_tone_map(vec3 color) {
  vec3 x = max(color - 0.004, 0);
  return (x*(6.2*x+0.5)) / (x*(6.2*x+1.7)+0.06);
}

void main() {
  vec3 color = bloom_fetch(ivec2(gl_FragCoord.xy));
  gl_FragColor = vec4(filmic_tone_map(color), 1.0);
}

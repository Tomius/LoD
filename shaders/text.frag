// Copyright (c) 2014, Tamas Csala

varying vec2 vTexcoord;
uniform sampler2D uTex;
uniform vec4 color;

void main() {
  gl_FragColor = vec4(vec3(1), texture2D(uTex, vTexcoord).r);
}

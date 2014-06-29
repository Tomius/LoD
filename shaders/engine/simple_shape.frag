// Copyright (c) 2014, Tamas Csala

#version 120

varying vec3 w_vNormal;

uniform vec3 uColor = vec3(1.0);

void main() {
  vec3 fake_lighting = mix(vec3(0.3), vec3(0.9), (1 + normalize(w_vNormal).y)/2);
  gl_FragColor = vec4(uColor * fake_lighting, 1);
}

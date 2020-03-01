// Copyright (c) 2014, Tamas Csala

#version 430

in vec3 w_vNormal;

uniform vec3 uColor = vec3(1.0);

out vec4 fragColor;

void main() {
  vec3 fake_light_pos = normalize(vec3(0.4, 0.8, 0.2));
  float dot_value = dot(normalize(w_vNormal), fake_light_pos);
  vec3 fake_lighting = mix(vec3(0.06), vec3(0.6), (1 + dot_value)/2);
  fragColor = vec4(uColor * fake_lighting, 1);
}

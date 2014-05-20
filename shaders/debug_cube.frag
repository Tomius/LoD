// Copyright (c) 2014, Tamas Csala

#version 120

uniform int uLevel;

void main() {
  if (uLevel == 0) {
    gl_FragColor = vec4(0.5, 0.0, 0.0, 1.0);
  } else if (uLevel == 1) {
    gl_FragColor = vec4(0.0, 0.5, 0.0, 1.0);
  } else if (uLevel == 2) {
    gl_FragColor = vec4(0.0, 0.0, 0.5, 1.0);
  } else if (uLevel == 3) {
    gl_FragColor = vec4(0.5, 0.5, 0.0, 1.0);
  } else if (uLevel == 4) {
    gl_FragColor = vec4(0.5, 0.0, 5.0, 1.0);
  } else if (uLevel == 5) {
    gl_FragColor = vec4(0.0, 0.5, 5.0, 1.0);
  } else if (uLevel == 6) {
    gl_FragColor = vec4(0.5, 0.5, 5.0, 1.0);
  } else {
    gl_FragColor = vec4(1.0);
  }
}

// Copyright (c) 2014, Tamas Csala

#version 120

#include "sky.frag"
#include "fog.frag"
#include "visibility_range_limit.frag"
#include "hemisphere_lighting.frag"

varying vec3 c_vPos;
varying vec3 w_vNormal;
varying vec2 vTexCoord;

uniform sampler2D uDiffuseTexture;

void main() {
  vec4 color = texture2D(uDiffuseTexture, vTexCoord);
  vec3 final_color = color.rgb * HemisphereLighting(w_vNormal);

  float actual_alpha = min(color.a, VisibilityRangeAlpha(c_vPos));
  if (actual_alpha < 1e-3) { discard; }

  gl_FragColor = vec4(ApplyFog(final_color, c_vPos), actual_alpha);
}

// Copyright (c) 2014, Tamas Csala

#version 120

#include "fog.frag"
#include "visibility_range_limit.frag"
#include "hemisphere_lighting.frag"

varying vec3 c_vPos;
varying vec3 w_vNormal;
varying vec2 vTexCoord;

uniform sampler2D uDiffuseTexture;

void main() {
  vec4 color = texture2D(uDiffuseTexture, vTexCoord);
  vec3 normal = normalize(w_vNormal);
  // Trees have fake normals, and they need fake lighting...
  vec3 lighting = 0.6*HemisphereLighting(normal) +
                  0.4*HemisphereLighting(-normal);
  vec3 final_color = color.rgb * lighting;

  float actual_alpha = min(color.a, VisibilityRangeAlpha(c_vPos));
  if (actual_alpha < 1e-1) { discard; }

  gl_FragColor = vec4(ApplyFog(final_color, c_vPos), actual_alpha);
}

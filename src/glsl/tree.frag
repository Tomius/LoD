// Copyright (c) 2014, Tamas Csala

#version 430

#include "fog.frag"
#include "visibility_range_limit.frag"
#include "hemisphere_lighting.frag"

in vec3 c_vPos;
in vec3 w_vNormal;
in vec2 vTexCoord;

uniform sampler2D uDiffuseTexture;

out vec4 fragColor;

void main() {
  vec4 color = texture2D(uDiffuseTexture, vTexCoord);
  vec3 normal = normalize(w_vNormal);
  // Trees have fake normals, and they need fake lighting...
  vec3 lighting = 0.6*HemisphereLighting(normal) +
                  0.4*HemisphereLighting(-normal);
  vec3 final_color = color.rgb * lighting;

  float actual_alpha = min(color.a, VisibilityRangeAlpha(c_vPos));
  if (actual_alpha < 1e-1) { discard; }

  fragColor = vec4(ApplyFog(final_color, c_vPos), actual_alpha);
}

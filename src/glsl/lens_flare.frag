// Copyright (c) 2014, Tamas Csala
// Lens flare based on musk's.
// See the original at: https://www.shadertoy.com/view/4sX3Rs

#version 130

#export vec3 LensFlare();

uniform vec2 uScreenSize;
uniform vec3 s_uSunPos;

vec3 LensFlare(vec2 uv, vec2 pos) {
  vec2 uvd = uv * length(uv);

  float f21 = max(0.75/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*0.25;
  float f22 = max(0.75/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*0.23;
  float f23 = max(0.75/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*0.21;

  vec2 uvx = mix(uv,uvd,-0.5);
  float f41 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
  float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
  float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;

  uvx = mix(uv,uvd,-.4);
  float f51 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
  float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
  float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;

  uvx = mix(uv,uvd,-0.5);
  float f61 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
  float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
  float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;

  vec3 c = vec3(0.0);
  c.r += f21 + f41 + f51 + f61;
  c.g += f22 + f42 + f52 + f62;
  c.b += f23 + f43 + f53 + f63;

  return 1.25*c;
}

vec3 LensFlare() {
  if (s_uSunPos.z < 0.0) {
    return vec3(0.0);
  }

  vec2 uv = gl_FragCoord.xy / uScreenSize - 0.5;
  uv.x *= uScreenSize.x/uScreenSize.y; // fix aspect ratio
  vec2 s_sun_pos = s_uSunPos.xy/2;
  s_sun_pos.x *= uScreenSize.x/uScreenSize.y; // fix aspect ratio

  return vec3(1.4, 1.2, 1.0) * LensFlare(uv, s_sun_pos.xy);
}

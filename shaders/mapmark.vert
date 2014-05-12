// Copyright (c) 2014, Tamas Csala

#version 120

attribute vec2 aPosition;
attribute vec2 aTexcoord;

uniform mat3 uModelMatrix;

varying vec2 vTexcoord;

void main() {
	vTexcoord = aTexcoord;
	gl_Position = vec4((uModelMatrix*vec3(aPosition, 1)).xy, 0, 1);
}

#version 330 core

in vec3 vertTexCoord;

out vec4 fragColor;

vec3 sky_color(vec3 lookDir);

void main() {
    fragColor = vec4(sky_color(normalize(vertTexCoord)), 1.0);
}

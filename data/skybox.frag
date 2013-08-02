#version 330

in vec3 vertTexCoord;

out vec3 fragColor;

vec3 sky_color(vec3 lookDir);

void main() {
    fragColor = sky_color(normalize(vertTexCoord));
}

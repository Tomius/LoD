#version 330 core

in vec3 Position;
uniform mat4 ProjectionMatrix;
uniform mat3 CameraMatrix;

out vec3 vertTexCoord;

void main(void) {
    gl_Position = ProjectionMatrix * vec4(CameraMatrix * vec3(10 * Position), 1.0);
    vertTexCoord = Position;
}

#version 330

in vec3 Position;
uniform mat4 ProjectionMatrix, CameraMatrix;

out vec3 vertTexCoord;

void main(void) {
    gl_Position = ProjectionMatrix * CameraMatrix * vec4(10 * Position, 1.0);
    vertTexCoord = Position;
}

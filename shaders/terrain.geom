#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertNormal[];
in vec3 vertCamPos[];
in vec3 vertWorldPos[];
in vec2 vertTexCoord[];

out vec3 normal;
out vec3 camPos;
out vec3 worldPos;
out vec2 texCoord;

void main() {
    // Culls vertices with invalid texture coordinates.
    for(int i = 0; i < 3; i++){
        if(1 <= abs(vertTexCoord[i].x) || 1 <= abs(vertTexCoord[i].y))
            return;
    }
    for(int i = 0; i < 3; i++) {
        normal = vertNormal[i];
        camPos = vertCamPos[i];
        worldPos = vertWorldPos[i];
        texCoord = vertTexCoord[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}

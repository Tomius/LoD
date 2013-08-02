#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 48) out;

in vec2 vertPos[];
uniform mat4 ProjectionMatrix, CameraMatrix;
uniform sampler2D HeightMap, NormalMap;

out vec3 normal;
out vec3 camPos;
out vec2 texCoord;

void make_point(vec2 Position) {
    ivec2 texSize = textureSize(HeightMap, 0);
    texCoord = ((ivec2(Position) + texSize / 2) % texSize) / vec2(texSize);
    float height = texture(HeightMap, texCoord).r * 512;

    normal = -texture(NormalMap, texCoord).rgb; // Have to flip it for some reason
    vec4 Pos = CameraMatrix * vec4(Position.x, height, Position.y, 1.0);
    camPos = Pos.xyz;

    gl_Position = ProjectionMatrix * Pos;
    EmitVertex();
}

void make_triangle(vec2 p0, vec2 p1, vec2 p2) {
    make_point(p0);
    make_point(p1);
    make_point(p2);

    EndPrimitive();
}

void do_tess_1(vec2 p_0, vec2 p_1, vec2 p_2, int l) {
    if(l == 1) make_triangle(p_0, p_1, p_2);
    else {
        vec2 p01 = (p_0 + p_1) / 2;
        vec2 p12 = (p_1 + p_2) / 2;
        vec2 p20 = (p_2 + p_0) / 2;
        make_triangle(p_0, p01, p20);
        make_triangle(p01, p_1, p12);
        make_triangle(p20, p12, p_2);
        make_triangle(p01, p12, p20);
    }
}

void do_tess_0(vec2 p_0, vec2 p_1, vec2 p_2, int l) {
    if(l == 0) make_triangle(p_0, p_1, p_2);
    else {
        vec2 p01 = (p_0 + p_1) / 2;
        vec2 p12 = (p_1 + p_2) / 2;
        vec2 p20 = (p_2 + p_0) / 2;
        do_tess_1(p_0, p01, p20, l);
        do_tess_1(p01, p_1, p12, l);
        do_tess_1(p20, p12, p_2, l);
        do_tess_1(p01, p12, p20, l);
    }
}

void main() {
    int TessLevel;
    float l = length((vertPos[0] + vertPos[1] + vertPos[2]) / 3);
    if(l < 32.0)
        TessLevel = 2;
    else if(l < 64.0)
        TessLevel = 1;
    else
        TessLevel = 0;
    do_tess_0(
        vertPos[0],
        vertPos[1],
        vertPos[2],
        TessLevel
    );
}

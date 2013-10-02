#version 330 core

in VertexData {
  vec3 texcoord;
};

out vec4 frag_color;

vec3 SkyColor(vec3 lookDir);

void main() {
    frag_color = vec4(SkyColor(normalize(texcoord)), 1.0);
}

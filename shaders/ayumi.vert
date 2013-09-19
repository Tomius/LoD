#version 330 core

in vec4 Position;
in vec2 TexCoord;
in vec3 Normal;
#define BONE_ATTRIB_NUM
in ivec4 BoneIDs[BONE_ATTRIB_NUM];
in vec4 Weights[BONE_ATTRIB_NUM];
uniform mat4 ProjectionMatrix, CameraMatrix, ModelMatrix;
#define BONE_NUM
uniform mat4 Bones[BONE_NUM];

out VertexData {
    vec3 c_normal;
    vec3 c_pos;
    vec2 texCoord;
} vert;

void main() {
    mat4 BoneTransform = mat4(0);
    for(int i = 0; i < BONE_ATTRIB_NUM; i++)
        for(int j = 0; j < 4; j++)
            BoneTransform += Bones[BoneIDs[i][j]] * Weights[i][j];

    vert.c_normal = vec3(CameraMatrix * ModelMatrix * BoneTransform * vec4(Normal, 0.0));
    vert.texCoord = TexCoord;
    vec4 cam_space_pos =
        CameraMatrix *
        (ModelMatrix *
        (BoneTransform *
        Position));
    vert.c_pos = vec3(cam_space_pos);

    gl_Position = ProjectionMatrix * cam_space_pos;
}

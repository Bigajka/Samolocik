#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 VertexColor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 FragPos;
out vec3 NormalWorld;
out vec3 ObjectColor;

void main()
{
    vec4 worldPosition = Model * vec4(Position, 1.0);

    FragPos = worldPosition.xyz;

    NormalWorld = mat3(transpose(inverse(Model))) * Normal;

    ObjectColor = VertexColor;

    gl_Position = Projection * View * worldPosition;
}
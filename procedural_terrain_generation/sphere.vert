// Shader-ul de varfuri / Vertex shader
#version 410 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec3 in_Normal;

out vec4 gl_Position;

uniform mat4 myMatrix;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
    gl_Position = projection * view * myMatrix * in_Position;
}
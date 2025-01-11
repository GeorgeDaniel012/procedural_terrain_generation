// Shader-ul de varfuri / Vertex shader
#version 410 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec2 in_Tex;

out vec2 TexCoord;

void main ()
{
   gl_Position = in_Position;
   TexCoord = in_Tex;
}
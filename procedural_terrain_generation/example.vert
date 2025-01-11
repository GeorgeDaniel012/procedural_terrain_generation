// Shader-ul de varfuri / Vertex shader
#version 410 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec2 in_Tex;

out vec2 TexCoord;

uniform mat4 myMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

//out vec4 Pos; 

void main ()
{
   //gl_Position = in_Position;
   gl_Position = projection*view*in_Position;
   //Pos = gl_Position;
   TexCoord = in_Tex;
}
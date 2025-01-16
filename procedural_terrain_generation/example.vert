// Shader-ul de varfuri / Vertex shader
#version 410 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec2 in_Tex;

out vec2 TexCoord;
out vec3 inViewPos;

uniform mat4 myMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

void main ()
{
   gl_Position = in_Position;
   inViewPos=vec3(projection*view*vec4(viewPos, 1.0f));
   TexCoord = in_Tex;
}
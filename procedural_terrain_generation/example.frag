// Shader-ul de fragment / Fragment shader 
#version 410 core

in float Height;
out vec4 out_Color;

void main(void)
{
    float h = (Height + 16)/64.0f;
    out_Color = vec4(h, h, h, 1.0);
}
// Shader-ul de fragment / Fragment shader 
#version 410 core

in vec4 ex_Color;
out vec4 out_Color;

void main(void)
  {
    vec4 col = (0.1, 0.1, 0.1, 1.0);
    out_Color = col;
  }
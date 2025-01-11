
// tessellation control shader
#version 410 core

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=4) out;

//in vec4 Pos[];
//out vec4 Pos1[];

// varying input from vertex shader
in vec2 TexCoord[];
// varying output to evaluation shader
out vec2 TextureCoord[];

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    //Pos1[gl_InvocationID] = Pos[gl_InvocationID];
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 16;
        gl_TessLevelOuter[1] = 16;
        gl_TessLevelOuter[2] = 16;
        gl_TessLevelOuter[3] = 16;

        gl_TessLevelInner[0] = 16;
        gl_TessLevelInner[1] = 16;
    }
}
/*

#version 410 core
#extension GL_ARB_tessellation_shader : enable

layout(vertices = 4) out;

in vec4 gl_Position[];
in vec2 TexCoord[];
out vec2 TextureCoord[];

void main()
{
    // Pass through vertex positions and texture coordinates
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    // Set tessellation levels
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 8;
        gl_TessLevelOuter[1] = 8;
        gl_TessLevelOuter[2] = 8;
        gl_TessLevelOuter[3] = 8;

        gl_TessLevelInner[0] = 8;
        gl_TessLevelInner[1] = 8;
    }
}
*/

// tessellation evaluation shader
#version 410 core

layout (quads, fractional_odd_spacing, cw) in;

uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform mat4 model;           // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix

//in vec4 Pos1[];
// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];

// send to Fragment Shader for coloring
//out float Height;
float Height;

void main()
{
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height and scale + shift as desired
    Height = texture(heightMap, texCoord).x * 64.0 - 16.0;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    //vec4 p00 = Pos1[0];//gl_in[0].gl_Position;
    //vec4 p01 = Pos1[1];//gl_in[1].gl_Position;
    //vec4 p10 = Pos1[2];//gl_in[2].gl_Position;
    //vec4 p11 = Pos1[3];//gl_in[3].gl_Position;
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    p += normal * Height;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    //gl_Position = projection * view * model * p;
    gl_Position = p;
}

/*
#version 410 core
#extension GL_ARB_tessellation_shader : enable

layout (quads, fractional_odd_spacing, ccw) in;

uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform mat4 model;           // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix

void main()
{
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position + gl_TessCoord.y * gl_in[1].gl_Position);// + gl_TessCoord.z * gl_in[2].gl_Position);
}*/

/*
#version 410 core
#extension GL_ARB_tessellation_shader : enable

layout(quads, equal_spacing, ccw) in;

in vec4 gl_Position[];
in vec2 TextureCoord[];
out vec2 FragTexCoord;

void main()
{
    // Interpolate vertex positions
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec3 p = mix(mix(p0, p1, u), mix(p2, p3, u), v);
    //gl_Position = vec4(p, 1.0);

    // Interpolate texture coordinates
    FragTexCoord = mix(mix(TextureCoord[0], TextureCoord[1], u), mix(TextureCoord[2], TextureCoord[3], u), v);
}*/
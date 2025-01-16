// Shader-ul de tessellation evaluation
#version 410 core

layout (quads, fractional_odd_spacing, cw) in;

uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform mat4 myMatrix;        // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];
in vec3 ViewPos[];

// send to Fragment Shader for coloring
out float Height;
out vec2 TexCoords;
out vec3 fragPos;
out vec3 inViewPos;

void main()
{
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // retrieve control point texture coordinates
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;
    TexCoords = texCoord;

    // lookup texel at patch coordinate for height and scale
    Height = texture(heightMap, texCoord).r * 64.0 - 16.0;

    // retrieve control point position coordinates
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
    p += normal * -Height;

    fragPos = (myMatrix * p).xyz;

    // bilinear interpolation of viewPos
    vec3 vp00 = ViewPos[0];
    vec3 vp01 = ViewPos[1];
    vec3 vp10 = ViewPos[2];
    vec3 vp11 = ViewPos[3];

    vec3 vp0 = mix(vp00, vp01, u);
    vec3 vp1 = mix(vp10, vp11, u);
    inViewPos = mix(vp0, vp1, v);

    // output patch
    gl_Position = projection * view * myMatrix * p;
}
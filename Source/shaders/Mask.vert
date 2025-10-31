#version 310 es

layout(std140) uniform vs_ub
{
    mat4 u_MVPMatrix;
} matrix;

layout(location = POSITION) in vec4 a_position;
layout(location = COLOR0) in vec4 a_color;
layout(location = TEXCOORD0) in vec2 a_texCoord;
layout(location = TEXCOORD1) in vec2 a_maskCoord;

layout(location = COLOR0) out vec4 v_color;
layout(location = TEXCOORD0) out vec2 v_texCoord;
layout(location = TEXCOORD1) out vec2 v_maskCoord;

void main()
{
    v_color = a_color;
    v_texCoord = a_texCoord;
    v_maskCoord = a_maskCoord;
    gl_Position = matrix.u_MVPMatrix * a_position;
}

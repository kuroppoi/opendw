#version 310 es

precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = TEXCOORD1) in vec2 v_maskCoord;

layout(location = SV_Target0) out vec4 FragColor;

uniform sampler2D u_texture;
uniform sampler2D u_maskTexture;

void main()
{
    vec4 texColor = texture(u_texture, v_texCoord);
    vec4 maskColor = texture(u_maskTexture, v_maskCoord);
    FragColor = vec4(texColor.rgb * maskColor.a * v_color.a, texColor.a * maskColor.a) * v_color;
}

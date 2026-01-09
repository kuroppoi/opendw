#version 310 es

precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

uniform sampler2D u_texture;

void main()
{
    vec4 color = vec4(0.0);
    vec2 blurSize = vec2(1.0 / 40.0);
    color += texture(u_texture, v_texCoord + vec2(-1.0, 0.0) * blurSize) * 0.20;
    color += texture(u_texture, v_texCoord + vec2(0.0, -1.0) * blurSize) * 0.20;
    color += texture(u_texture, v_texCoord + vec2(1.0, 0.0) * blurSize) * 0.20;
    color += texture(u_texture, v_texCoord + vec2(0.0, 1.0) * blurSize) * 0.20;
    color += texture(u_texture, v_texCoord) * 0.12;
    FragColor = color * v_color;
}

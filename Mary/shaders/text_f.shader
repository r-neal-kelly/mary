#version 330 core

in vec2 v_uv;
out vec4 f_color;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
  f_color = vec4(1, 1, 1, texture(u_texture, v_uv).b) * u_color;
}

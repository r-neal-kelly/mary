#version 330 core

layout(location = 0) in vec2 xy;
layout(location = 1) in vec2 uv;
out vec2 v_uv;

uniform mat4 u_projection;

void main()
{
  gl_Position = u_projection * vec4(xy, 0, 1);
  v_uv = uv;
}

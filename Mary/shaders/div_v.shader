#version 330 core

layout(location = 0) in vec2 xy;

uniform mat4 u_projection;

void main()
{
  gl_Position = u_projection * vec4(xy, 0, 1);
}

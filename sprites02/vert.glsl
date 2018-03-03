#version 410

layout (location=0) in vec2 a_position;

uniform mat4 u_model;
uniform mat4 u_projection;

void main() {
  vec4 pos = vec4(a_position.x, a_position.y, 0.0, 1.0);
  gl_Position = u_projection * u_model * pos;
}

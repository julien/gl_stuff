#version 410

layout (location=0) in vec2 a_position;

uniform mat4 u_matrix;

void main() {
  vec4 pos = vec4(a_position, 1.0, 1.0);
  gl_Position = u_matrix * pos;
}

#version 410

layout (location=0) in vec2 a_position;
// layout (location=1) in vec2 a_texcoord;

uniform mat4 u_matrix;

void main() {
  vec4 pos = vec4(a_position.x, a_position.y, 0.0, 1.0);
  gl_Position = pos * u_matrix;
}

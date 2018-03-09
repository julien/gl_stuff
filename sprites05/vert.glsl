#version 410
layout (location=0) in vec2 a_position;
layout (location=1) in vec4 a_color;
out vec4 v_color;
uniform mat4 u_matrix;
void main() {
  gl_Position = u_matrix * vec4(a_position, 1.0, 1.0);
  v_color = a_color;
}

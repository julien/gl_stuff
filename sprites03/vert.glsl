#version 410

layout (location=0) in vec2 a_position;
// layout (location=1) in vec2 a_texcoord;

// out vec2 v_texcoord;
uniform mat4 u_matrix;

void main() {
  // v_texcoord = a_texcoord;
  vec4 pos = vec4(a_position, 1.0, 1.0);
  gl_Position = u_matrix * pos;
}

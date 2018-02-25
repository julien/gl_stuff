#version 410

layout (location=0) in vec3 a_position;
layout (location=1) in vec2 a_texcoord;

out vec2 v_texcoord;
uniform mat4 u_matrix;

void main() {
  vec4 pos = vec4(a_position.x, a_position.y, a_position.z, 1.0);
  v_texcoord = a_texcoord;
  gl_Position = pos * u_matrix;
};

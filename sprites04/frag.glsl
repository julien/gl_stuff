#version 410

uniform sampler2D u_image;
in vec2 v_uv;
in vec4 v_color;
out vec4 frag_color;

void main() {
  // frag_color = texture(u_image, v_uv) * v_color;
  frag_color = texture(u_image, v_uv);
}

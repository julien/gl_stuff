#version 410

uniform sampler2D u_image;
in vec2 v_texcoord;
out vec4 frag_color;

void main() {
  frag_color = texture(u_image, v_texcoord);
  // frag_color = vec4(1.0, 1.0, 1.0, 1.0);
}

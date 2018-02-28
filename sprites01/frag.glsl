#version 410

uniform sampler2D u_image;
in vec2 v_texcoord;
out vec4 frag_color;

void main() {
  vec4 texel = texture(u_image, v_texcoord);
  if (texel.a < 0.5) discard;
  frag_color = texel;
}

#version 410

layout (location=0) in float a_rotation;
layout (location=1) in vec2 a_translation;
layout (location=2) in vec2 a_scale;
layout (location=3) in vec2 a_position;
layout (location=4) in vec2 a_uv;
layout (location=5) in vec4 a_color;

out vec2 v_uv;
out vec4 v_color;

uniform mat4 u_matrix;

void main() {
  float ax = cos(a_rotation);
  float ay = sin(a_rotation);


  vec2 xy = (vec2(
			  a_position.x * ax - a_position.y * ay,
			  a_position.x * ay + a_position.y * ax
			) * a_scale) + a_translation;

  vec4 pos = vec4(xy, 1.0, 1.0);
  gl_Position = u_matrix * pos;

  v_uv = a_uv;
  v_color = a_color;
}

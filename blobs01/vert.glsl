#version 410

layout (location=0) in vec2 a_position;
layout (location=1) in float a_pointsize;

void main() {
  gl_Position = vec4(a_position, 0.0, 1.0);
  gl_PointSize = a_pointsize;
}

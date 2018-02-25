#version 410

in vec2 vp;
uniform vec2 u_resolution;
uniform float u_time;
uniform float u_pointsize;

void main() {
	gl_PointSize = u_pointsize;

	vec2 zeroToOne = vp / u_resolution;
	vec2 zeroToTwo = zeroToOne * 2.0;
	vec2 clipSpace = zeroToTwo - 1.0;

	gl_Position = vec4(clipSpace * vec2(1.0, -1.0), 0.0, 1.0);
};

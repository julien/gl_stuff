#version 410

layout(location=0) in vec3 squareVertices;
layout(location=1) in vec4 xyzs;

uniform vec2 u_resolution;
uniform float u_time;

void main() {
	vec2 vp = xyzs.xy + squareVertices.xy;
	vec2 zeroToOne = vp / u_resolution;
	vec2 zeroToTwo = zeroToOne * 2.0;
	vec2 clipSpace = zeroToTwo - 1.0;

	gl_Position = vec4(clipSpace * vec2(1.0, -1.0), 0.0, 1.0);
}

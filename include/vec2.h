#ifndef __VEC2_H__
#define __VEC2_H__

#include <math.h>

struct vec2 {
	float x;
	float y;
};

void vec2_add(vec2 *v1, vec2 v2) {
	v1->x += v2.x;
	v1->y += v2.y;
}

float vec2_angle(vec2 v1, vec2 v2) {

	if (v1.x == 0 && v1.y == 0 && v2.x == 0 && v2.y == 0) return 0.0f;

	double dot = v1.x*v2.x + v1.y*v2.y;
	double v1mag = sqrt(v1.x*v1.x + v1.y*v1.y);
	double v2mag = sqrt(v2.x*v2.x + v2.y*v2.y);
	double amt = dot / (v1mag*v2mag);

	if (amt <= -1) {
		return M_PI;
	} else if (amt >= 1) {
		return 0;
	}
	return (float) acos(amt);
}

float vec2_dist(vec2 v1, vec2 v2) {
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	return (float) sqrt(dx*dx + dy*dy);
}

void vec2_div(vec2 *v, float n) {
	v->x /= n;
	v->y /= n;
}

float vec2_dot(vec2 v1, vec2 v2) {
	return v1.x*v2.x + v1.y*v2.y;
}

vec2 vec2_from_angle(float angle) {
	return {
		(float) cos(angle),
		(float) sin(angle)
	};
}

float vec2_get_mag(vec2 v) {
	return (float) sqrt(v.x*v.x + v.y*v.y);
}

float vec2_get_magsq(vec2 v) {
	return (v.x*v.x + v.y*v.y);
}

float vec2_heading(vec2 v) {
	return (float) atan2(v.x, v.y);
}

void vec2_mul(vec2 *v, float n) {
	v->x *= n;
	v->y *= n;
}

void vec2_normalize(vec2 *v) {
	float m = vec2_get_mag(*v);
	if (m != 0 && m != 1) {
		v->x /= m;
		v->y /= m;
	}
}

void vec2_limit(vec2 *v, float max) {
	if (vec2_get_magsq(*v) > max * max) {
		vec2_normalize(v);
		v->x *= max;
		v->y *= max;
	}
}

void vec2_rotate(vec2 *v, float theta) {
	float t = v->x;

	v->x = v->x * cos(theta) - v->y * sin(theta);
	v->y = t * sin(theta) + v->y * cos(theta);
}

void vec2_set(vec2 *v, float x, float y) {
	v->x = x;
	v->y = y;
}

void vec2_set_mag(vec2 *v, float len) {
	vec2_normalize(v);
	v->x *= len;
	v->y *= len;
}

void vec2_sub(vec2 *v1, vec2 v2) {
	v1->x -= v2.x;
	v1->y -= v2.y;
}

#endif

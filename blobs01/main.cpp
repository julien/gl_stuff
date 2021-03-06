#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

struct blob {
	float x, y, r;
	float nx, ny, nr;
	float ex, ey, er;
};

int g_viewport_width = 1024;
int g_viewport_height = 768;

const int max_blobs = 5;
blob *blobs = NULL;

const int max_points = 40000;
const int point_size = 7;
const int num_points = max_points * point_size;
float *points = NULL;

blob *create_blobs() {
	blob *blobs = (blob*) malloc(sizeof(blob) * max_blobs);
	if (blobs == NULL) {
		fprintf(stderr, "Couln't allocate memory for blobs\n");
		return NULL;
	}

	for (int i = 0; i < max_blobs; i++) {
		blobs[i].x = 0.0 + (i * 0.1);
		blobs[i].y = 0.0 + (i * 0.1);
		blobs[i].r = rand_range(0.1, 3) * 0.1;
		blobs[i].nx = rand_range(-10, 10) * 0.1;
		blobs[i].ny = rand_range(-10, 10) * 0.1;
		blobs[i].nr = rand_range(0.1, 3) * 0.1;
		blobs[i].ex = rand_range(2, 6) * 0.01;
		blobs[i].ey = rand_range(2, 6) * 0.01;
		blobs[i].er = rand_range(2, 6) * 0.01;
	}

	return blobs;
}

void update_blobs(blob *bs) {
	for (int i = 0; i < max_blobs; i++) {
		blob *b = &(bs[i]);

		float dx = (b->nx - b->x);
		float dy = (b->ny - b->y);
		float dr = (b->nr - b->r);

		float ax = fabs(dx);
		float ay = fabs(dy);
		float ar = fabs(dr);

		float vx = dx * b->ex;
		float vy = dy * b->ey;
		float vr = dr * b->er;

		b->x += vx;
		b->y += vy;
		b->r += vr;

		if (ax < 0.01 && ay < 0.01 && ar < 0.01) {
			b->ex = rand_range(2, 6) * 0.009;
			b->ey = rand_range(2, 6) * 0.009;
			b->er = rand_range(2, 6) * 0.01;

			b->nx = rand_range(-10, 10) * 0.1;
			b->ny = rand_range(-10, 10) * 0.1;
			b->nr = rand_range(1, 30) * 0.01;
		}
	}
}

void create_points() {
	points = (float*) malloc(sizeof(float) * num_points);
	if (points == NULL) {
		fprintf(stderr, "Couln't allocate memory for points\n");
		return;
	}

	for (int i = 0; i < num_points; i += point_size) {
		// position
		points[i]   = rand_range(-10, 10) * 0.1;
		points[i+1] = rand_range(-10, 10) * 0.1;

		// pointsize
		points[i+2] = (10 + rand() % 80) * 0.1;

		// vx,vy
		points[i+3] = rand_range(-3, 3) * 0.0001;
		points[i+4] = rand_range(-3, 3) * 0.0001;
		// ax,ay
		points[i+5] = rand_range(-2, -9) * 0.1;
		points[i+6] = rand_range(-2, -9) * 0.1;
	}
}

void update_points() {
	for (int i = 0; i < num_points; i += point_size) {
		float x = points[i];
		float y = points[i+1];
		float r = points[i+2];
		float vx = points[i+3];
		float vy = points[i+4];
		float ax = points[i+5];
		float ay = points[i+6];

		// velocity += acceleration
		vx += ax;
		vy += ay;

		// reset acceleration
		ax *= 0;
		ay *= 0;

		for (int k = 0; k < max_blobs; k++) {
			blob b = blobs[k];
			float dx = (x) - b.x;
			float dy = (y) - b.y;
			float dist = sqrt(dx*dx + dy*dy);

			if (dist < b.r) {
				x = b.x + dx / dist * b.r;
				y = b.y + dy / dist * b.r;
				r = (10 + rand() % 40) * 0.1;
			}
		}

		// position += velocity
		x += vx * 0.01;
		y += vy * 0.01;

		if (x > 1.0) {
			x = -1.0;
		} else if (x < -1.0) {
			x = 1.0;
		}
		if (y > 1.0) {
			y = -1.0;
		} else if (y < -1.0) {
			y = 1.0;
		}

		points[i] =   x;
		points[i+1] = y;
		points[i+2] = r;
	}
}

int main() {
	srand(time(NULL));

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(
			g_viewport_width, g_viewport_height, "  ", NULL, NULL);

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(mon);

	int x = (int)((mode->width - g_viewport_width) * 0.5);
	int y = (int)((mode->height - g_viewport_height) * 0.5);

	glfwSetWindowPos(window, x, y);

	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	GLuint sp = create_program("vert.glsl", "frag.glsl");

	blobs = create_blobs();
	create_points();

	GLuint vp_vbo;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_points * sizeof(GLfloat), points, GL_STATIC_DRAW);

	int fsize = sizeof(float);
	int stride = fsize * point_size;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
	glEnableVertexAttribArray(0);

	                                                         // (sizeof(float) * 2 == 2 * 4)
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (void*) 8);
	glEnableVertexAttribArray(1);

	glUseProgram(sp);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window)) {

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update_blobs(blobs);
		update_points();

		glBufferSubData(GL_ARRAY_BUFFER, 0, num_points * sizeof(GLfloat), points);

		glDrawArrays(GL_POINTS, 0, max_points);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
	}

	if (blobs != NULL) free(blobs);
	if (points != NULL) free(points);

	glfwTerminate();
	return 0;
}

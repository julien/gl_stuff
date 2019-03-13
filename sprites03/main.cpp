#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

struct sprite {
	float ax;
	float ay;
	float life;
	float size;
	float vx;
	float vy;
	float x;
	float y;
};

int g_viewport_width = 1024;
int g_viewport_height = 768;
const unsigned int MAX_SPRITES = 1000;
const int SPRITE_MESH_SIZE = 12;
const int SPRITE_SIZE = 2;
unsigned int freesprite = 0;
unsigned int spritecount = 0;
sprite sprites[MAX_SPRITES];
GLfloat vpos_data[MAX_SPRITES * SPRITE_MESH_SIZE];
GLfloat view_matrix[16] = {
	2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 0.0f
};
const float delta = 0.05;
const float grav = 3.0;
GLfloat mat[] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

void setup_sprites() {
	for (size_t i = 0; i < MAX_SPRITES; i++) {
		sprite *s = &(sprites[i]);
		float w = (float) (g_viewport_width * 0.5);
		float h = (float) (g_viewport_height * 0.5);
		float x = w + rand_range(-10, 10);
		float y = h + rand_range(-10, 10);
		s->ax = 0;
		s->ay = 0;
		s->life = rand_range(100, 200);
		s->size = SPRITE_SIZE + rand_range(2, SPRITE_SIZE);
		s->vx = rand_range(-1, 1);
		s->vy = rand_range(-1, 1);
		s->x = x;
		s->y = y;
	}
}

int find_free_sprite() {
	for (unsigned int i = freesprite; i < MAX_SPRITES; i++) {
		if (sprites[i].life < 0) {
			freesprite = i;
			return i;
		}
	}
	for (unsigned int i = 0; i < freesprite; i++) {
		if (sprites[i].life < 0) {
			freesprite = i;
			return i;
		}
	}
	return -1;
}

void draw_rect(float x, float y, float w, float h) {
	float a = mat[0];
	float b = mat[1];
	float c = mat[2];
	float d = mat[3];
	float e = mat[4];
	float f = mat[5];
	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;
	float x2 = x;
	float y2 = y + h;
	float x3 = x;
	float y3 = y;
	float x4 = x + w;
	float y4 = y;
	float x5 = x + w;
	float y5 = y + h;
	int offset = 0;

	offset = spritecount * SPRITE_MESH_SIZE;

	vpos_data[offset++] = x0 * a + y0 * c + e;
	vpos_data[offset++] = x0 * b + y0 * d + f;

	vpos_data[offset++] = x1 * a + y1 * c + e;
	vpos_data[offset++] = x1 * b + y1 * d + f;

	vpos_data[offset++] = x2 * a + y2 * c + e;
	vpos_data[offset++] = x2 * b + y2 * d + f;

	vpos_data[offset++] = x3 * a + y3 * c + e;
	vpos_data[offset++] = x3 * b + y3 * d + f;

	vpos_data[offset++] = x4 * a + y4 * c + e;
	vpos_data[offset++] = x4 * b + y4 * d + f;

	vpos_data[offset++] = x5 * a + y5 * c + e;
	vpos_data[offset++] = x5 * b + y5 * d + f;

	if (++spritecount >= MAX_SPRITES) {
		glBufferData(GL_ARRAY_BUFFER,
				spritecount * SPRITE_MESH_SIZE * sizeof (GLfloat),
				vpos_data, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, spritecount * 6);
		spritecount = 0;
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

	int wx = (int)((mode->width - g_viewport_width) * 0.5);
	int wy = (int)((mode->height - g_viewport_height) * 0.5);

	glfwSetWindowPos(window, wx, wy);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	GLuint sp = create_program("vert.glsl", "frag.glsl");
	glUseProgram(sp);

	GLint u_matrix = glGetUniformLocation(sp, "u_matrix");
	glUniformMatrix4fv(u_matrix, 1, GL_FALSE, view_matrix);

	setup_sprites();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vp_vbo;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window)) {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);
		glClear(GL_COLOR_BUFFER_BIT);

		for (size_t i = 0; i < MAX_SPRITES; i++) {
			if (sprites[i].life > 0) {
				sprites[i].ay += grav;

				/* sprites[i].ax *= delta; */
				/* sprites[i].ay *= delta; */

				sprites[i].vx += sprites[i].ax;
				sprites[i].vy += sprites[i].ay;

				sprites[i].x += sprites[i].vx;
				sprites[i].y += sprites[i].vy;
				sprites[i].ax = 0;
				sprites[i].ay = 0;

				sprites[i].life--;

				draw_rect(sprites[i].x, sprites[i].y, sprites[i].size, sprites[i].size);

			} else {
				float size = SPRITE_SIZE + rand_range(SPRITE_SIZE * 0.5, SPRITE_SIZE);
				float vx = rand_range(-2, 2);
				float vy = rand_range(-2, 2);
				float x = (g_viewport_width * 0.5) + rand_range(-20, 20);
				float y = (g_viewport_height * 0.5) + rand_range(-20, 20);
				sprites[i].life = rand_range(100, 200);
				sprites[i].size = size;
				sprites[i].vx = vx;
				sprites[i].vy = vy;
				sprites[i].x = x;
				sprites[i].y = y;
			}
		}

		glBufferSubData(GL_ARRAY_BUFFER, 0,
				spritecount * SPRITE_MESH_SIZE * sizeof(GLfloat),
				vpos_data);
		glDrawArrays(GL_TRIANGLES, 0, spritecount * 6);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

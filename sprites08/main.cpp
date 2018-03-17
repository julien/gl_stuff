#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"
#include "vec2.h"

#define SPRITE_COUNT 1000000
#define SPRITE_MAX_FORCE 20
#define SPRITE_MAX_SPEED 10
#define MIN_FLEE_DISTANCE 60
#define MAX_FLEE_DISTANCE 400

int g_viewport_width = 800;
int g_viewport_height = 800;

GLfloat view_matrix[16] = {
	2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
	0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 0.0f
};

static const size_t vertcount = 6;
static float rgba[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static size_t buffidx = 0;
static float *vposdata = NULL;
static float *vposcurr = NULL;
static float *vcoldata = NULL;
static float *vcolcurr = NULL;
static float *vuvdata = NULL;
static float *vuvcurr = NULL;
static GLuint vao;
static GLuint posvbo;
static GLuint colvbo;
static GLuint uvvbo;
static size_t vpossize;
static size_t vcolsize;
static size_t vuvsize;

vec2 mouse;
float flee_distance = MIN_FLEE_DISTANCE;

void setcol(float r, float g, float b, float a = 1.0f) {
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

void draw(float x, float y, float w, float h) {
	/* 1st triangle */
	vposcurr[0] = x;
	vposcurr[1] = y;
	vposcurr[2] = x + w;
	vposcurr[3] = y + h;
	vposcurr[4] = x;
	vposcurr[5] = y + h;
	/* 2nd triangle */
	vposcurr[6] = x;
	vposcurr[7] = y;
	vposcurr[8] = x + w;
	vposcurr[9] = y;
	vposcurr[10] = x + w;
	vposcurr[11] = y + h;

	/* 1st triangle */
	vcolcurr[0] = rgba[0];
	vcolcurr[1] = rgba[1];
	vcolcurr[2] = rgba[2];
	vcolcurr[3] = rgba[3];
	vcolcurr[4] = rgba[0];
	vcolcurr[5] = rgba[1];
	vcolcurr[6] = rgba[2];
	vcolcurr[7] = rgba[3];
	vcolcurr[8] = rgba[0];
	vcolcurr[9] = rgba[1];
	vcolcurr[10] = rgba[2];
	vcolcurr[11] = rgba[3];
	/* 2nd triangle */
	vcolcurr[12] = rgba[0];
	vcolcurr[13] = rgba[1];
	vcolcurr[14] = rgba[2];
	vcolcurr[15] = rgba[3];
	vcolcurr[16] = rgba[0];
	vcolcurr[17] = rgba[1];
	vcolcurr[18] = rgba[2];
	vcolcurr[19] = rgba[3];
	vcolcurr[20] = rgba[0];
	vcolcurr[21] = rgba[1];
	vcolcurr[22] = rgba[2];
	vcolcurr[23] = rgba[3];

	/* 1st triangle */
	vuvcurr[0] = 0;
	vuvcurr[1] = 1;
	vuvcurr[2] = 1;
	vuvcurr[3] = 0;
	vuvcurr[4] = 0;
	vuvcurr[5] = 0;
	/* 2nd triangle */
	vuvcurr[6] = 0;
	vuvcurr[7] = 1;
	vuvcurr[8] = 1;
	vuvcurr[9] = 1;
	vuvcurr[10] = 1;
	vuvcurr[11] = 0;

	vposcurr = (float*)((char*) vposcurr + (sizeof(float) * 12));
	vcolcurr = (float*)((char*) vcolcurr + (sizeof(float) * 24));
	vuvcurr = (float*)((char*) vuvcurr + (sizeof(float) * 12));

	++buffidx;
}

void flush() {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, posvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		sizeof(float) * buffidx * vertcount * 2, vposdata);

	glBindBuffer(GL_ARRAY_BUFFER, colvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		sizeof(float) * buffidx * vertcount * 4, vcoldata);

	glBindBuffer(GL_ARRAY_BUFFER, uvvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		sizeof(float) * buffidx * vertcount * 2, vuvdata);

	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(buffidx * vertcount));

	buffidx = 0;
	vposcurr = vposdata;
	vcolcurr = vcoldata;
	vuvcurr = vuvdata;
}

/* sprites */
struct sprites {
	float ax[SPRITE_COUNT];
	float ay[SPRITE_COUNT];
	float cr[SPRITE_COUNT];
	float cg[SPRITE_COUNT];
	float cb[SPRITE_COUNT];
	float px[SPRITE_COUNT];
	float py[SPRITE_COUNT];
	float sx[SPRITE_COUNT];
	float sy[SPRITE_COUNT];
	float vx[SPRITE_COUNT];
	float vy[SPRITE_COUNT];

	vec2 target[SPRITE_COUNT];

	size_t count;
};

void init_sprites(sprites *s) {
	float hw = (float) g_viewport_width * 0.5;
	float hh = (float) g_viewport_height * 0.5;

	for (size_t i = 0; i < SPRITE_COUNT; i++) {
		s->ax[i] = 0;
		s->ay[i] = 0;
		s->px[i] = hw + rand_range(-300, 300);
		s->py[i] = hh + rand_range(-300, 300);
		s->vx[i] = 0;
		s->vx[i] = 0;

		s->target[i] = vec2{s->px[i], s->py[i]};

		/* colors */
		s->cr[i] = rand_range(1, 10) * 0.1f;
		s->cg[i] = rand_range(1, 10) * 0.1f;
		s->cb[i] = rand_range(1, 10) * 0.1f;

		float size = 3 + rand_range(0, 12);
		s->sx[i] = s->sy[i] = size;
	}
	s->count = 3000;
}

void apply_sprite_force(sprites *s, size_t i, vec2 f) {
	if (i < SPRITE_COUNT) {
		s->ax[i] += f.x;
		s->ay[i] += f.y;
	}
}

vec2 sprite_flee(sprites *s, size_t i, vec2 t) {
	vec2 f;

	if (i < SPRITE_COUNT) {
		vec2 desired = {
			t.x - s->px[i],
			t.y - s->py[i]
		};

		float d = vec2_get_mag(desired);
		if (d < flee_distance) {
			vec2_set_mag(&desired, SPRITE_MAX_SPEED);
			desired.x *= -1;
			desired.y *= -1;

			f.x = desired.x - s->vx[i];
			f.y = desired.y - s->vy[i];
			vec2_limit(&f, SPRITE_MAX_FORCE);
		}
	}

	return f;
}

vec2 sprite_arrive(sprites *s, size_t i, vec2 t) {
	vec2 f;

	if (i < SPRITE_COUNT) {

		vec2 desired = {
			t.x - s->px[i],
			t.y - s->py[i]
		};

		float speed = SPRITE_MAX_SPEED;
		float d = vec2_get_mag(desired);
		if (d < 100) {
			speed = map(d, 0, 100, 0, SPRITE_MAX_SPEED);
		}
		vec2_set_mag(&desired, speed);

		f.x = desired.x - s->vx[i];
		f.y = desired.y - s->vy[i];

		vec2_limit(&f, SPRITE_MAX_FORCE);
	}

	return f;
}

void apply_behaviors(sprites *s) {
	for (size_t i = 0; i < s->count; i++) {
		vec2 arrive = sprite_arrive(s, i, s->target[i]);
		arrive.x *= 0.2;
		arrive.y *= 0.2;


		vec2 flee = sprite_flee(s, i, mouse);
		flee.x *= 0.2;
		flee.y *= 0.2;

		apply_sprite_force(s, i, arrive);
		apply_sprite_force(s, i, flee);
	}
}

void update_sprites(sprites *s) {
	for (size_t i = 0; i < s->count; i++) {
		s->px[i] += s->vx[i];
		s->py[i] += s->vy[i];

		s->vx[i] += s->ax[i];
		s->vy[i] += s->ay[i];


		s->ax[i] = 0;
		s->ay[i] = 0;
	}
}

void render_sprites(sprites *s) {
	for (size_t i = 0; i < s->count; i++) {
		setcol(s->cr[i], s->cg[i], s->cb[i]);
		draw(s->px[i], s->py[i], s->sx[i], s->sy[i]);
	}
}


void init_buffers() {
	vpossize = SPRITE_COUNT * (sizeof(float) * 12);
	vcolsize = SPRITE_COUNT * (sizeof(float) * 24);
	vuvsize = SPRITE_COUNT * (sizeof(float) * 12);

	vposdata = (float*) malloc(vpossize);
	vposcurr = vposdata;

	vcoldata = (float*) malloc(vcolsize);
	vcolcurr = vcoldata;

	vuvdata = (float*) malloc(vuvsize);
	vuvcurr = vuvdata;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &posvbo);
	glBindBuffer(GL_ARRAY_BUFFER, posvbo);
	glBufferData(GL_ARRAY_BUFFER, vpossize, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &colvbo);
	glBindBuffer(GL_ARRAY_BUFFER, colvbo);
	glBufferData(GL_ARRAY_BUFFER, vcolsize, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &uvvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvvbo);
	glBufferData(GL_ARRAY_BUFFER, vuvsize, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
	mouse.x = xpos;
	mouse.y = ypos;
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
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	sprites *s = (sprites*) malloc(sizeof(sprites));
	if (NULL == s) {
		fprintf(stderr, "Couldn't allocate memory for sprites\n");
		return 1;
	}

	mouse.x = 0;
	mouse.y = 0;

	init_sprites(s);

	GLuint sp = create_program("vert.glsl", "frag.glsl");
	glUseProgram(sp);

	init_buffers();

	GLint u_matrix = glGetUniformLocation(sp, "u_matrix");
	glUniformMatrix4fv(u_matrix, 1, GL_FALSE, view_matrix);

	GLint u_image = glGetUniformLocation(sp, "u_image");
	GLuint tex;
	load_texture("ball.png", &tex, 0);
	glUniform1i(u_image, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window)) {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT);

		update_sprites(s);
		apply_behaviors(s);
		render_sprites(s);
		flush();

		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) {
			if (s->count + 100 < SPRITE_COUNT) {
				s->count += 100;
			}
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) {
			if (s->count - 100 > 1) {
				s->count -= 100;
			}
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT)) {
			if (flee_distance < MAX_FLEE_DISTANCE) {
				flee_distance++;
			}
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT)) {
			if (flee_distance > MIN_FLEE_DISTANCE) {
				flee_distance--;
			}
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
	}

	if (NULL != s)
		free(s);

	glfwTerminate();
	return 0;
}

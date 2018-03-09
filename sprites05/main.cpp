#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

#define SPRITE_COUNT 100000

int g_viewport_width = 1024;
int g_viewport_height = 768;

GLfloat view_matrix[16] = {
    2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 0.0f
};

static const size_t vertcount = 6;
static const size_t max_vertices = SPRITE_COUNT * vertcount;
static float rgba[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static size_t buffidx = 0;
static float *vposdata = nullptr;
static float *vposcurr = nullptr;
static float *vcoldata = nullptr;
static float *vcolcurr = nullptr;
static GLuint vao;
static GLuint posvbo;
static GLuint colvbo;
static size_t vpossize;
static size_t vcolsize;
static float gravity = 1.5f;

void setcol(float r, float g, float b, float a = 1.0f) {
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

void draw(float x, float y, float w, float h) {
	// 1st triangle
	vposcurr[0] = x;
	vposcurr[1] = y;
	vposcurr[2] = x + w;
	vposcurr[3] = y + h;
	vposcurr[4] = x;
	vposcurr[5] = y + h;
	// 2nd triangle
	vposcurr[6] = x;
	vposcurr[7] = y;
	vposcurr[8] = x + w;
	vposcurr[9] = y;
	vposcurr[10] = x + w;
	vposcurr[11] = y + h;

	// 1st triangle
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

	// 2nd triangle
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

	vposcurr = (float*)((char*) vposcurr + (sizeof(float) * 12));
	vcolcurr = (float*)((char*) vcolcurr + (sizeof(float) * 24));

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

	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(buffidx * vertcount));
	buffidx = 0;
	vposcurr = vposdata;
	vcolcurr = vcoldata;
}

/* sprites */
struct sprites {
	float px[SPRITE_COUNT];
	float py[SPRITE_COUNT];
	float vx[SPRITE_COUNT];
	float vy[SPRITE_COUNT];
	float cr[SPRITE_COUNT];
	float cg[SPRITE_COUNT];
	float cb[SPRITE_COUNT];
	float sx[SPRITE_COUNT];
	size_t count;
};

void init_sprites(sprites *s) {
	for (size_t i = 0; i < SPRITE_COUNT; i++) {
		s->px[i] = (g_viewport_width * 0.5) + rand_range(-10, 10);
		s->py[i] = rand_range(-10, 10);
		s->vx[i] = rand_range(-10, 10) * sin((float)i);
		s->vy[i] = rand_range(-10, 10) * sin((float)i);
		s->cr[i] = rand_range(1, 10) * 0.1f;
		s->cg[i] = rand_range(1, 10) * 0.1f;
		s->cb[i] = rand_range(1, 10) * 0.1f;
		s->sx[i] = 2 + (int)rand_range(0, 4);
	}
	s->count = 0;
}

void update_sprites(sprites *s) {
	for (size_t i = 0; i < s->count; i++) {
		s->vy[i] += gravity;
		s->py[i] += s->vy[i];
		s->px[i] += s->vx[i];

		if (s->py[i] > g_viewport_height) {
			s->py[i] = g_viewport_height;
			// s->vy[i] *= -1.0f;
			s->vy[i] *= -0.9f;
		} else if (s->py[i] < 0) {
			s->py[i] = 0;
			// s->vy[i] *= -1.0f;
			s->vy[i] *= -0.8f;
		}
		if (s->px[i] > g_viewport_width) {
			s->px[i] = g_viewport_width;
			// s->vx[i] *= -1.0f;
			s->vx[i] *= -0.9f;
		} else if (s->px[i] < 0) {
			s->px[i] = 0;
			// s->vx[i] *= -1.0f;
			s->vx[i] *= -0.9f;
		}

		s->vy[i]--;
	}
}

void render_sprites(sprites *s) {
	for (size_t i = 0; i < s->count; i++) {
		setcol(s->cr[i], s->cg[i], s->cb[i]);
		draw(s->px[i], s->py[i], s->sx[i], s->sx[i]);
	}
}

void init_buffers() {
	vpossize = SPRITE_COUNT * (sizeof(float) * 12);
	vcolsize = SPRITE_COUNT * (sizeof(float) * 24);

	vposdata = (float*) malloc(vpossize);
	vposcurr = vposdata;

	vcoldata = (float*) malloc(vcolsize);
	vcolcurr = vcoldata;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	glGenBuffers(1, &posvbo);
	glBindBuffer(GL_ARRAY_BUFFER, posvbo);
	glBufferData(GL_ARRAY_BUFFER, vpossize, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
   	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);


	glGenBuffers(1, &colvbo);
	glBindBuffer(GL_ARRAY_BUFFER, colvbo);
	glBufferData(GL_ARRAY_BUFFER, vcolsize, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
   	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
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

	sprites *s = (sprites*) malloc(sizeof(sprites));
	if (NULL == s) {
		fprintf(stderr, "Couldn't allocate memory for sprites\n");
		return 1;
	}
	init_sprites(s);

    GLuint sp = create_program("vert.glsl", "frag.glsl");
    glUseProgram(sp);

	init_buffers();

    GLint u_matrix = glGetUniformLocation(sp, "u_matrix");
    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, view_matrix);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    while (!glfwWindowShouldClose(window)) {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClear(GL_COLOR_BUFFER_BIT);

		update_sprites(s);
		render_sprites(s);
		flush();

        glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) {
			if (s->count < SPRITE_COUNT) {
				s->count += 100;
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

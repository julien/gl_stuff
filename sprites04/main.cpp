#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;
const unsigned int VERTEX_SIZE = 9;
const unsigned int MAX_BATCH = 10922;
const unsigned int VERTEX_DATA_SIZE = VERTEX_SIZE * MAX_BATCH * 6;
const unsigned int VERTICES_PER_QUAD = 6;
unsigned int count = 0;
GLfloat pdata[VERTEX_DATA_SIZE];
GLfloat view_matrix[16] = {
    2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 0.0f
};
float mousex = (float)(g_viewport_width * 0.5);
float mousey = (float)(g_viewport_height * 0.5);
struct sprite {
	float ax;
	float ay;
	float life;
	float rotation;
	float rotation_speed;
	float size;
	float vx;
	float vy;
	float x;
	float y;
};

const unsigned int MAX_SPRITES = 100000;
const unsigned int SPRITE_SIZE = 8;
unsigned int freesprite = 0;
unsigned int spritecount = 0;
sprite sprites[MAX_SPRITES];
const float delta = 0.075;
const float grav = 1.8;

int get_free_sprite() {
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

void init_sprites() {
	for (size_t i = 0; i < MAX_SPRITES; i++) {
		sprite *s = &(sprites[i]);
		float x = mousex + rand_range(-10, 10);
		float y = mousey + rand_range(-10, 10);
		s->ax = 0;
		s->ay = 0;
		s->life = -1.0;
		s->size = SPRITE_SIZE + rand_range(2, SPRITE_SIZE);
		s->vx = rand_range(-10, 10);
		s->vy = rand_range(-10, 10);
		s->x = x;
		s->y = y;
		s->rotation = 0.0f;
		s->rotation_speed = rand_range(-1, 1) * 0.01;
	}
}

void flush() {
	if (0 == count) return;
	/* TODO: optimize this (glBufferSubData);    */
	/* TODO: use index buffer and glDrawElements */
	glBufferData(GL_ARRAY_BUFFER,
			VERTEX_DATA_SIZE,
			pdata,
			GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, count * VERTICES_PER_QUAD);
	count = 0;
}

/* TODO: add texture argument */
void draw(float x, float y, float w, float h, float r,
		float tx, float ty, float sx, float sy,
		float u0, float v0, float u1, float v1) {

	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y;
	float x2 = x;
	float y2 = y + h;
	float x3 = x;
    float y3 = y + h;
	float x4 = x + w;
	float y4 = y;
	float x5 = x + w;
	float y5 = y + h;

    unsigned int offset = count * VERTEX_SIZE * VERTICES_PER_QUAD;

    /* Rotation | Translation | Scale | Position | UV */
	/* Vertex 1 */
	pdata[offset++] = r;
	pdata[offset++] = tx;
	pdata[offset++] = ty;
	pdata[offset++] = sx;
	pdata[offset++] = sy;
	pdata[offset++] = x0;
	pdata[offset++] = y0;
	pdata[offset++] = u0;
	pdata[offset++] = v1;

	/* Vertex 2 */
	pdata[offset++] = r;
	pdata[offset++] = tx;
	pdata[offset++] = ty;
	pdata[offset++] = sx;
	pdata[offset++] = sy;
	pdata[offset++] = x1;
	pdata[offset++] = y1;
	pdata[offset++] = u1;
	pdata[offset++] = v1;

	/* Vertex 3 */
	pdata[offset++] = r;
	pdata[offset++] = tx;
	pdata[offset++] = ty;
	pdata[offset++] = sx;
	pdata[offset++] = sy;
	pdata[offset++] = x2;
	pdata[offset++] = y2;
	pdata[offset++] = u0;
	pdata[offset++] = v0;

	/* Vertex 4 */
	pdata[offset++] = r;
	pdata[offset++] = tx;
	pdata[offset++] = ty;
	pdata[offset++] = sx;
	pdata[offset++] = sy;
	pdata[offset++] = x3;
	pdata[offset++] = y3;
	pdata[offset++] = u0;
	pdata[offset++] = v0;

	/* Vertex 5 */
	pdata[offset++] = r;
	pdata[offset++] = tx;
	pdata[offset++] = ty;
	pdata[offset++] = sx;
	pdata[offset++] = sy;
	pdata[offset++] = x4;
	pdata[offset++] = y4;
	pdata[offset++] = u1;
	pdata[offset++] = v1;

	/* Vertex 6 */
	pdata[offset++] = r;
	pdata[offset++] = tx;
	pdata[offset++] = ty;
	pdata[offset++] = sx;
	pdata[offset++] = sy;
	pdata[offset++] = x5;
	pdata[offset++] = y5;
	pdata[offset++] = u1;
	pdata[offset++] = v0;

    if (++count >= MAX_BATCH) {
		flush();
    }
}


void init_buffers() {
    /* Rotation | Translation | Scale | Position | UV */
    int fsize = sizeof(GLfloat);
    int stride = fsize * 9;

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    /* Rotation */
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, stride, NULL);
    glEnableVertexAttribArray(0);
    /* Translation */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*) 4);
    glEnableVertexAttribArray(1);
    /* Scale */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*) 12);
    glEnableVertexAttribArray(2);
    /* Position */
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*) 20);
    glEnableVertexAttribArray(3);
    /* UV */
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, stride, (void*) 28);
    glEnableVertexAttribArray(4);
}

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
	mousex = xpos;
	mousey = ypos;
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

	glfwSetCursorPosCallback(window, cursor_pos_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    GLuint sp = create_program("vert.glsl", "frag.glsl");
    glUseProgram(sp);

	init_buffers();

    /* Rendering settings */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint u_matrix = glGetUniformLocation(sp, "u_matrix");
    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, view_matrix);

    GLint u_image = glGetUniformLocation(sp, "u_image");
    GLuint tex;
    load_texture("heart.png", &tex, 0);
    glUniform1i(u_image, 0);

	init_sprites();

    glClearColor(0.0, 0.0, 0.0, 1.0);

    while (!glfwWindowShouldClose(window)) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		int newsprites = (int) (elapsed_seconds * 10000.0);
		if (newsprites >= 5)
			newsprites = 5;

		for (int i = 0; i < newsprites; i++) {
			float x = mousex + rand_range(-10, 10);
			float y = mousey + rand_range(-10, 10);

			float size = SPRITE_SIZE + rand_range(1, SPRITE_SIZE);
			float rotation = 0.0; /* rand_range(-9, 9) * 0.01; */

			int idx = get_free_sprite();
			sprites[idx].x = x;
			sprites[idx].y = y;
			sprites[idx].rotation = rotation;
			sprites[idx].rotation_speed = rand_range(-1, 1) * 0.01;
			sprites[idx].size = size;
			sprites[idx].life = 100;
		}

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClear(GL_COLOR_BUFFER_BIT);

		for (size_t i = 0; i < MAX_SPRITES; i++) {
			if (sprites[i].life > 0) {
				sprites[i].ay += grav;

				sprites[i].ax *= delta;
				sprites[i].ay *= delta;

				sprites[i].vx += sprites[i].ax;
				sprites[i].vy += sprites[i].ay;

				sprites[i].x += sprites[i].vx;
				sprites[i].y += sprites[i].vy;
				sprites[i].ax = 0;
				sprites[i].ay = 0;

				sprites[i].rotation += sprites[i].rotation_speed;

				sprites[i].life--;

				draw(sprites[i].x, sprites[i].y,
					sprites[i].size, sprites[i].size,
					sprites[i].rotation,
					0, 0,
					1, 1,
					0, 0, 1, 1);
			} else {
				sprites[i].life = -1.0f;
			}
		}

		flush();

        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

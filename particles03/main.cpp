#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"
#include "particle.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;
ParticlePool *particles;

GLfloat view_matrix[] = {
	2.0f, 0.0f, 0.0f, -1.0f,
	0.0f, 2.0f, 0.0f,  1.0f,
	0.0f, 0.0f, 1.0f,  0.0f,
	0.0f, 0.0f, 0.0f,  1.0f,
};

GLint u_matrix = -1;

int main() {
	srand(time(NULL));

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow *window = glfwCreateWindow(
			g_viewport_width, g_viewport_height, "  ", NULL, NULL);

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(mon);

	int wx = (int)((mode->width - g_viewport_width) * 0.5);
	int wy = (int)((mode->height - g_viewport_height) * 0.5);

	glfwSetWindowPos(window, wx, wy);

	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	particles = new ParticlePool();

	GLuint sp = create_program("vert.glsl", "frag.glsl");
	glUseProgram(sp);

	u_matrix = glGetUniformLocation(sp, "u_matrix");
	GLint u_image = glGetUniformLocation(sp, "u_image");

	view_matrix[0] *= 1.0f / (float) g_viewport_width;
	view_matrix[5] *= -1.0f / (float) g_viewport_height;

	glUniformMatrix4fv(u_matrix, 1, GL_FALSE, view_matrix);

	GLuint tex;
	load_texture("heart.png", &tex, 0);
	glUniform1i(u_image, 0);

	GLuint vp_vbo;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);

	int fsize = sizeof(GLfloat);
	int stride = fsize * 5;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, NULL);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*) 12);
	glEnableVertexAttribArray(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window)) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		int newparticles = (int) (elapsed_seconds * 10000.0);
		if (newparticles >= (int) (0.016f*1000.0))
			newparticles = (int) (0.016f * 1000.0);

		for (int i = 0; i < newparticles; i++) {
			float x = (g_viewport_width * 0.5) + rand_range(-10, 10);
			float y = (g_viewport_height * 0.5) + rand_range(-10, 10);
			float size = 8 + rand_range(2, 20);
			float vx = rand_range(-10, 10);
			float vy = rand_range(-10, 10);

			particles->create(x, y, size, size, vx, vy, 100);
		}

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT);

		particles->update();

		int quads = particles->draw();

		glBufferData(GL_ARRAY_BUFFER,
			MAX_PARTICLES * 30 * sizeof(GLfloat),
			particles->get_vertices(),
			GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, quads);

		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
	}

	delete particles;

	glfwTerminate();
	return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int g_viewport_width = 1024;
int g_viewport_height = 768;

int main() {
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

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	const char *frag_src =
		"#version 410\n"
		"out vec4 frag_color;"
		"void main() {"
		"  frag_color = vec4(1.0, 1.0, 1.0, 1.0);"
		"}";

	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &frag_src, NULL);
	glCompileShader(frag);

	int params = -1;
	glGetShaderiv(frag, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "Error shader %i didn't compile\n", frag);
		return -1;
	}

	GLuint prog = glCreateProgram();

	glAttachShader(prog, frag);
	glLinkProgram(prog);

	params = -1;
	glGetProgramiv(prog, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "Error: couldn't link shader program %u\n", prog);
		return -1;
	}

	glDeleteShader(frag);
	glUseProgram(prog);

	GLuint points[] = {
		-1.0, 1.0,
		-1.0, 1.0,
		 1.0, 1.0,
		-1.0, 1.0,
		 1.0, 1.0,
		 1.0, -1.0,
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) * 6, points, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window)) {

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/* 	glBufferSubData(GL_ARRAY_BUFFER, 0, num_points * sizeof(GLfloat), points); */

		glDrawArrays(GL_POINTS, 0, 6);

		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;
unsigned const VERTEX_SIZE = (4 * sizeof(GLfloat));

GLfloat view_matrix[16] = {
	2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 0.0f
};

int main() {
	srand( time( NULL ) );

	glfwInit();

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	GLFWwindow *window = glfwCreateWindow(
			g_viewport_width, g_viewport_height, "  ", NULL, NULL );

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode( mon );

	int wx = (int)((mode->width - g_viewport_width) * 0.5);
	int wy = (int)((mode->height - g_viewport_height) * 0.5);

	glfwSetWindowPos( window, wx, wy );
	glfwMakeContextCurrent( window );

	glewExperimental = GL_TRUE;
	glewInit();

	GLuint sp = create_program( "vert.glsl", "frag.glsl" );
	glUseProgram( sp );

	GLint u_matrix = glGetUniformLocation( sp, "u_matrix" );
	glUniformMatrix4fv( u_matrix, 1, GL_FALSE, view_matrix );

	GLint u_image = glGetUniformLocation( sp, "u_image" );
	GLuint tex;
	// load_texture( "dude.png", &tex, 0 );
	// glUniform1i( u_image, 0 );

	// Buffers
	// rotation|translation|scale|position|uv|color
	int fsize = sizeof( GLfloat );
	int stride = fsize * 5;

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	GLuint r_vbo, t_vbo, s_vbo, p_vbo, uv_vbo, c_vbo;
	glGenBuffers( 1, &r_vbo );
	glGenBuffers( 1, &t_vbo );
	glGenBuffers( 1, &s_vbo );
	glGenBuffers( 1, &p_vbo );
	glGenBuffers( 1, &uv_vbo );
	glGenBuffers( 1, &c_vbo );

	// rotation
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 1, GL_FLOAT, GL_FALSE, stride, NULL );
	// translation
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride * 2, (void*) 2 );
	// scale
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, stride * 4, (void*) 6 );
	// position
	glEnableVertexAttribArray( 3 );
	glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, stride * 6, (void*) 10 );
	// uv
	glEnableVertexAttribArray( 4 );
	glVertexAttribPointer( 4, 2, GL_FLOAT, GL_FALSE, stride * 8, (void*) 14 );
	// color
	glEnableVertexAttribArray( 5 );
	glVertexAttribPointer( 5, 4, GL_FLOAT, GL_FALSE, stride * 10, (void*) 18 );

	// Rendering settings
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	while ( !glfwWindowShouldClose( window ) ) {
		// static double previous_seconds = glfwGetTime();
		// double current_seconds = glfwGetTime();
		// double elapsed_seconds = current_seconds - previous_seconds;
		// previous_seconds = current_seconds;

		int w, h;
		glfwGetFramebufferSize( window, &w, &h );
		glViewport( 0, 0, w, h );

		glClear( GL_COLOR_BUFFER_BIT );

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	glfwTerminate();
	return 0;
}

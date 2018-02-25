#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;

GLuint create_shaders() {
	const char *vs_str = "#version 410\n"
		"in vec2 vp;"
		"uniform vec2 u_resolution;"
		"uniform float u_time;"
		"uniform float u_pointsize;"
		"void main() {"
		"  gl_PointSize = u_pointsize;"
		"  vec2 zeroToOne = vp / u_resolution;"
		"  vec2 zeroToTwo = zeroToOne * 2.0;"
		"  vec2 clipSpace = zeroToTwo - 1.0;"
		"  gl_Position = vec4(clipSpace * vec2(1.0, -1.0), 0.0, 1.0);"
		"}";
	const char *fs_str = "#version 410\n"
		"out vec4 frag_color;"
		"void main() {"
		"  frag_color = vec4(1.0, 1.0, 1.0, 0.25);"
		"}";

	GLuint vs = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vs, 1, &vs_str, NULL );
	glCompileShader( vs );

	GLint params = -1;
	glGetShaderiv( vs, GL_COMPILE_STATUS, &params );
	if ( GL_TRUE != params ) {
		fprintf( stderr, "ERROR: GL shader index %i did not compile\n", vs );
		print_infolog( vs );
		return - 1;
	}

	GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fs, 1, &fs_str, NULL );
	glCompileShader( fs );

	params = -1;
	glGetShaderiv( fs, GL_COMPILE_STATUS, &params );
	if ( GL_TRUE != params ) {
		fprintf( stderr, "ERROR: GL shader index %i did not compile\n", vs );
		print_infolog( fs );
		return -1;
	}

	GLuint sp = glCreateProgram();
	glAttachShader( sp, vs );
	glAttachShader( sp, fs );
	glLinkProgram( sp );

	params = -1;
	glGetProgramiv( sp, GL_LINK_STATUS, &params );
	if ( GL_TRUE != params ) {
		fprintf( stderr, "ERROR: couldn't link shader program %u\n", sp );
		print_infolog( sp );
		return -1;
	}

	glDeleteShader( vs );
	glDeleteShader( fs );

	return sp;
}

struct particle {
	float x;
	float y;
	float w;
	float h;
	float vx;
	float vy;
	float life;
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

	int x = (int)((mode->width - g_viewport_width) * 0.5);
	int y = (int)((mode->height - g_viewport_height) * 0.5);

	glfwSetWindowPos( window, x, y );

	glfwSetFramebufferSizeCallback( window, glfw_framebuffer_size_callback );
	glfwMakeContextCurrent( window );

	glewExperimental = GL_TRUE;
	glewInit();

	particle p = {
		(float)g_viewport_width * 0.5f,
		(float)g_viewport_height * 0.5f,
		(float)g_viewport_width,
		(float)g_viewport_height,
		(float)rand_range(-2, 2),
		(float)rand_range(-2, 2),
		100.0f,
	};

	size_t num_points = 1;
	size_t point_size = 2;
	size_t points_length = num_points * point_size;
	GLfloat *points = (float*) malloc( sizeof( float ) * points_length );
	if ( points == NULL ) {
		fprintf( stderr, "ERROR: Could not allocate memory for points\n" );
		return 1;
	}

	points[0] = p.x;
	points[1] = p.y;

	GLuint vp_vbo;
	glGenBuffers( 1, &vp_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vp_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( points ) * num_points, points, GL_STATIC_DRAW );

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	GLuint sp = create_shaders();

	glUseProgram( sp );

	GLint u_resolution = glGetUniformLocation( sp, "u_resolution" );
	GLint u_time = glGetUniformLocation( sp, "u_time" );
	GLint u_pointsize = glGetUniformLocation( sp, "u_pointsize" );

	glUniform2f( u_resolution, g_viewport_width, g_viewport_height );
	glUniform1f( u_pointsize, 4.0f );

	glEnable( GL_PROGRAM_POINT_SIZE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	glfw_framebuffer_size_callback( window, g_viewport_width, g_viewport_height );

	while ( !glfwWindowShouldClose( window ) ) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		previous_seconds = current_seconds;

		int speedX = rand_range( -5, 5 );
		int speedY = rand_range( -5, 5 );

		p.x += speedX;
		p.y += speedY;

		if ( p.x > p.w ) {
			p.x = 0;
		} else if ( p.x < 0 ) {
			p.x = p.w;
		}
		if ( p.y > p.h ) {
			p.y = 0;
		} else if ( p.y < 0 ) {
			p.y = p.h;
		}

		int old_length = (num_points * point_size);
		int new_length = old_length + point_size;

		points = (float*) realloc( points, sizeof( float ) * new_length );
		if ( points == NULL ) {
			fprintf( stderr, "ERROR: Could not reallocate memory for %d points\n", new_length );
			exit( 1 );
		}

		points[old_length] = p.x;
		points[old_length + 1] = p.y;
		num_points++;

		int w, h;
		glfwGetFramebufferSize( window, &w, &h );
		glViewport( 0, 0, w, h );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUniform2f( u_resolution, g_viewport_width, g_viewport_height );
		glUniform1f( u_time, previous_seconds );

		glBufferData( GL_ARRAY_BUFFER, sizeof( points ) * num_points, points, GL_STATIC_DRAW );
		glBindVertexArray( vao );
		glDrawArrays( GL_POINTS, 0, num_points );

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	// clean up
	glDisable( GL_PROGRAM_POINT_SIZE );

	if ( points != NULL ) {
		free( points );
	}

	glfwTerminate();
	return 0;
}

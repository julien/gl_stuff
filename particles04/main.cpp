#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <glm/vec3.hpp>
// #include <glm/vec4.hpp>
// #include <glm/mat4x4.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "utils.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;

GLfloat view_matrix[] = {
	2.0f, 0.0f, 0.0f, -1.0f,
	0.0f, 2.0f, 0.0f,  1.0f,
	0.0f, 0.0f, 1.0f,  0.0f,
	0.0f, 0.0f, 0.0f,  1.0f,
};

GLint u_matrix = -1;

struct particle {
	vec2 pos;
	vec2 vel;
	vec2 acc;
	float size;
	float life;
};

const int MAX_PARTICLES = 1000;
const int PARTICLE_MESH_SIZE = 6;
int free_particle = 0;
particle particles[MAX_PARTICLES];
float particles_vertices[MAX_PARTICLES * PARTICLE_MESH_SIZE];

int get_free_particle() {
	for (  int i = free_particle; i < MAX_PARTICLES; i++ )  {
		if ( particles[i].life < 0 ) {
			free_particle = i;
			return i;
		}
	}
	for (  int i = 0; i < free_particle; i++ )  {
		if ( particles[i].life < 0 ) {
			free_particle = i;
			return i;
		}
	}
	return 0;
}

int draw_particles() {
	int quads = 0;
	int quads6 = 0;

	for ( int i = 0; i < MAX_PARTICLES; i++ ) {
		if ( particles[i].life > 0 ) {

			int quads6i = quads6;

			vec2 pos = particles[i].pos;
			float size = particles[i].size;

			float x = pos.x;
			float y = pos.y;

			particles_vertices[quads6i++] = x;
			particles_vertices[quads6i++] = y - (size * 0.5);

			particles_vertices[quads6i++] = x - (size * 0.5);
			particles_vertices[quads6i++] = y + (size * 0.5);

			particles_vertices[quads6i++] = x + (size * 0.5);
			particles_vertices[quads6i++] = y + (size * 0.5);

			quads6 += 6;
			quads++;
		}
	}

	return quads * 6;
}

int main() {
	srand( time( NULL ) );

	glfwInit();

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	GLFWwindow *window = glfwCreateWindow(
			g_viewport_width, g_viewport_height, "  ", NULL, NULL );

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode( mon );

	int wx = (int)((mode->width - g_viewport_width) * 0.5);
	int wy = (int)((mode->height - g_viewport_height) * 0.5);

	glfwSetWindowPos( window, wx, wy );

	glfwSetFramebufferSizeCallback( window, glfw_framebuffer_size_callback );
	glfwMakeContextCurrent( window );

	glewExperimental = GL_TRUE;
	glewInit();

	for ( int i = 0; i < MAX_PARTICLES; i++) {
		particles[i].pos.x = g_viewport_width * 0.5;// rand_range( 0, g_viewport_width );
		particles[i].pos.y = g_viewport_height * 0.5;// rand_range( 0, g_viewport_height );
		particles[i].vel.x = rand_range( -2, 2 );
		particles[i].vel.y = rand_range( -2, 2 );
		particles[i].size = 4 + rand_range( 8, 12 );
		particles[i].life = -1;
	}

	particles[0].life = 100;

	GLuint sp = create_program( "vert.glsl", "frag.glsl" );
	glUseProgram( sp );

	u_matrix = glGetUniformLocation( sp, "u_matrix" );
	GLint u_image = glGetUniformLocation( sp, "u_image" );

	view_matrix[0] *= 1.0f / (float) g_viewport_width;
	view_matrix[5] *= -1.0f / (float) g_viewport_height;


	// TODO: replace with glm::mat4
	glUniformMatrix4fv( u_matrix, 1, GL_FALSE, view_matrix );

	// glUniformMatrix4fv( u_matrix, 1, GL_FALSE,
	// 		(const float*) glm::value_ptr( view_matrix ) );

	GLuint vp_vbo;
	glGenBuffers( 1, &vp_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vp_vbo );

	// int fsize = sizeof( GLfloat );
	// int stride = fsize * 2;

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// TODO: use for alpha attrib
	// glVertexAttribPointer( 1, 1, GL_FLOAT, GL_FALSE, stride, (void*) 8);
	// glEnableVertexAttribArray( 1 );

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

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		int quads = draw_particles();
		glBufferData( GL_ARRAY_BUFFER,
			MAX_PARTICLES * PARTICLE_MESH_SIZE * sizeof( GLfloat ),
			particles_vertices,
			GL_DYNAMIC_DRAW );
		glDrawArrays( GL_TRIANGLES, 0, quads );

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	glfwTerminate();
	return 0;
}

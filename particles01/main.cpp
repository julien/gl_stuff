#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include "utils.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;

struct particle {
	float x, y, z;
	float vx, vy;
	float size, life;
};

const int max_particles = 50000;
int last_used_particle = 0;
particle particles[max_particles];

int find_free_particle() {
	for (int i = last_used_particle; i < max_particles; i++) {
		if (particles[i].life < 0) {
			last_used_particle = i;
			return i;
		}
	}

	for (int i = 0; i < last_used_particle; i++) {
		if (particles[i].life < 0) {
			last_used_particle = i;
			return i;
		}
	}

	return 0;
}

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

	for ( int i = 0; i < max_particles; i++ ) {
		particles[i].life = -1.0f;
		particles[i].x = rand_range( 0, g_viewport_width );
		particles[i].y = rand_range( 0, g_viewport_height );
		particles[i].z = 0.0f;
	}

	static GLfloat* g_particles_position_size_data = new GLfloat[max_particles * 4]; // x, y, z, w

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	GLuint vertex_vbo;
	glGenBuffers( 1, &vertex_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vertex_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_STATIC_DRAW );

	GLuint position_vbo;
	glGenBuffers( 1, &position_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, position_vbo );
	// initialize with NULL buffer, it will be updated on each frame
	glBufferData( GL_ARRAY_BUFFER, max_particles * 4 * sizeof( GLfloat ), NULL, GL_STREAM_DRAW );

	GLuint sp = create_program( "vert.glsl", "frag.glsl" );
	glUseProgram( sp );

	GLint u_resolution = glGetUniformLocation( sp, "u_resolution" );
	GLint u_time = glGetUniformLocation( sp, "u_time" );

	glUniform2f( u_resolution, g_viewport_width, g_viewport_height );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	glfw_framebuffer_size_callback( window, g_viewport_width, g_viewport_height );

	while ( !glfwWindowShouldClose( window ) ) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		int newparticles = (int) (elapsed_seconds * 10000.0);
		if ( newparticles >= (int) (0.016f*10000.0) )
			newparticles = (int) (0.016f * 10000.0);

		for ( int i = 0; i < newparticles; i++ ) {
			int index = find_free_particle();
			particles[index].vx = rand_range( -2.0f, 2.0f );
			particles[index].vy = rand_range( -2.0f, 2.0f );
			particles[index].life = rand_range( 0, 100 );
			particles[index].size = 1.0f;
		}

		int particles_count = 0;
		for ( int i = 0; i < max_particles; i++ ) {
			particle& p = particles[i];

			if (p.life > 0.0f) {
				p.x += p.vx;
				p.y += p.vy;

				if ( p.x > g_viewport_width ) {
					p.x = 0;
				} else if ( p.x < 0 ) {
					p.x = g_viewport_width;
				}
				if ( p.y > g_viewport_height ) {
					p.y = 0;
				} else if ( p.y < 0 ) {
					p.y = g_viewport_height;
				}

				g_particles_position_size_data[4 * particles_count + 0] = p.x;
				g_particles_position_size_data[4 * particles_count + 1] = p.y;
				g_particles_position_size_data[4 * particles_count + 2] = p.z;
				g_particles_position_size_data[4 * particles_count + 3] = p.size;

				p.life -= 0.01f;
			} else {
				p.x = rand_range( 0, g_viewport_width );
				p.y = rand_range( 0, g_viewport_height );
				p.vx = rand_range( -2, 2 );
				p.vy = rand_range( -2, 2 );
				p.life = rand_range( 0, 100 );
			}

			particles_count++;
		}

		glBindBuffer( GL_ARRAY_BUFFER, position_vbo );
		glBufferData( GL_ARRAY_BUFFER, max_particles * 4 * sizeof( GLfloat ), NULL, GL_STREAM_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, particles_count * sizeof( GLfloat) * 4, g_particles_position_size_data );

		int w, h;
		glfwGetFramebufferSize( window, &w, &h );
		glViewport( 0, 0, w, h );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUniform2f( u_resolution, g_viewport_width, g_viewport_height );
		glUniform1f( u_time, previous_seconds );

		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, vertex_vbo );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, position_vbo );
		glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0 );

		glVertexAttribDivisor( 0, 0 );
		glVertexAttribDivisor( 1, 1 );

		glDrawArraysInstanced( GL_TRIANGLE_STRIP, 0, 4, particles_count );

		glDisableVertexAttribArray( 0 );
		glDisableVertexAttribArray( 1 );

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	glDisable( GL_PROGRAM_POINT_SIZE );

	delete[] g_particles_position_size_data;

	glfwTerminate();
	return 0;
}

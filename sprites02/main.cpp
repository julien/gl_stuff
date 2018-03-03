#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "utils.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;

const int MAX_SPRITES = 30;
int free_sprite = 0;

struct sprite {
	float life;
	float rotation;
	glm::vec2 acceleration;
	glm::vec2 position;
	glm::vec2 size;
	glm::vec2 velocity;
};

sprite sprites[MAX_SPRITES];

void initBuffers(GLuint *vao) {

	GLfloat vertices[6] = {
		 0.0, -1.0,
		-1.0,  1.0,
		 1.0,  1.0
	};
	GLuint vbo;

	glGenVertexArrays( 1, vao );
	glGenBuffers(1, &vbo );

	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	glBindVertexArray( *vao );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( GLfloat ), (GLvoid*) 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}

void draw_sprite( GLuint vao, GLint model_location, glm::vec2 position,
		glm::vec2 size, GLfloat rotate ) {
	glm::mat4 model;

	model = glm::translate( model, glm::vec3( position, 1.0f ) );

	model = glm::translate( model, glm::vec3( 0.5 * size.x, 0.5f * size.y, 0.0 ) );
	model = glm::rotate( model, rotate, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	model = glm::translate( model, glm::vec3( -0.5 * size.x, -0.5f * size.y, 0.0 ) );

	model = glm::scale( model, glm::vec3( size, 1.0f ) );
	glUniformMatrix4fv( model_location, 1, GL_FALSE, (const float*) glm::value_ptr( model ) );

	glBindVertexArray( vao );
	glDrawArrays( GL_TRIANGLES, 0, 3 );
	glBindVertexArray( 0 );
}


int get_free_sprite() {
	for (  int i = free_sprite; i < MAX_SPRITES; i++ )  {
		if ( sprites[i].life < 0 ) {
			free_sprite = i;
			return i;
		}
	}
	for (  int i = 0; i < free_sprite; i++ )  {
		if ( sprites[i].life < 0 ) {
			free_sprite = i;
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
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

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

	GLuint u_model = glGetUniformLocation( sp, "u_model" );
	GLuint u_projection = glGetUniformLocation( sp, "u_projection" );

	glm::mat4 projection = glm::ortho( 0.0f,
			(float) g_viewport_width, (float) g_viewport_height,
			0.0f, -1.0f, 0.0f );

	glUniformMatrix4fv( u_projection, 1, GL_FALSE, (const float*) glm::value_ptr( projection ) );

	for ( int i = 0; i < MAX_SPRITES; i++) {
		sprites[i].position = glm::vec2(
			( (float) g_viewport_width * 0.5) + rand_range( -200.0f, 200.0f ),
			( (float) g_viewport_height * 0.5) + rand_range( -200.0f, 200.0f )
		);

		float size = 5.0f + rand_range( 10.0f, 20.0f );
		sprites[i].size = glm::vec2( size, size );
		sprites[i].life = 1.0f;
	}

	GLuint vao;
	initBuffers(&vao);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	while ( !glfwWindowShouldClose( window ) ) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		int w, h;
		glfwGetFramebufferSize( window, &w, &h );
		glViewport( 0, 0, w, h );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		for ( int i = 0; i < MAX_SPRITES; i++ ) {
			if ( sprites[i].life > 0) {
				sprites[i].rotation += elapsed_seconds;

				draw_sprite( vao, u_model, sprites[i].position, sprites[i].size,
						sprites[i].rotation );
			}
		}

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	glfwTerminate();
	return 0;
}

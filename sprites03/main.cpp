#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

struct sprite {
	float ax;
	float ay;
	float life;
	float size;
	float vx;
	float vy;
	float x;
	float y;
	// TODO: add color?
};

int g_viewport_width = 1024;
int g_viewport_height = 768;

const unsigned int MAX_SPRITES = 300;
const int SPRITE_MESH_SIZE = 12;
const int SPRITE_SIZE = 10;
unsigned int free_sprite = 0;

sprite sprites[MAX_SPRITES];
GLfloat sprite_vertices[MAX_SPRITES * SPRITE_MESH_SIZE];

GLfloat view_matrix[16] = {
	2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 0.0f
};

void setup_sprites() {
	for ( size_t i = 0; i < MAX_SPRITES; i++ ) {
		sprite *s = &( sprites[i] );
		float w = (float) (g_viewport_width * 0.5);
		float h = (float) (g_viewport_height * 0.5);
		float x = w + rand_range( -w, w );
		float y = h + rand_range( -h, h );
		s->x = x;
		s->y = y;
		s->life = 1.0;
		s->size = SPRITE_SIZE + rand_range( 4, 8 );
		s->vx = 0;
		s->vy = 0;
		s->ax = 0;
		s->ay = 0;
	}
}

int find_free_sprite() {
	for (unsigned int i = free_sprite; i < MAX_SPRITES; i++) {
		if (sprites[i].life < 0) {
			free_sprite = i;
			return i;
		}
	}
	for (unsigned int i = 0; i < free_sprite; i++) {
		if (sprites[i].life < 0) {
			free_sprite = i;
			return i;
		}
	}
	return -1;
}

// TODO: use matrix for translation, rotation, scale
int draw_sprites() {
	int quads = 0;
	int quadsi = 0;

	for ( unsigned int i = 0; i < MAX_SPRITES; i++ ) {
		sprite *s = &(sprites[i]);

		if (s->life > 0) {

			int quadsii = quadsi;
			float x = s->x;
			float y = s->y;
			float xx = x + s->size;
			float yy = y + s->size;

			sprite_vertices[quadsii++] = x;
			sprite_vertices[quadsii++] = y;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 1;

			sprite_vertices[quadsii++] = xx;
			sprite_vertices[quadsii++] = y;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 1;
			// sprite_vertices[quadsii++] = 1;

			sprite_vertices[quadsii++] = x;
			sprite_vertices[quadsii++] = yy;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 0;

			sprite_vertices[quadsii++] = x;
			sprite_vertices[quadsii++] = yy;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 0;

			sprite_vertices[quadsii++] = xx;
			sprite_vertices[quadsii++] = y;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 1;
			// sprite_vertices[quadsii++] = 1;

			sprite_vertices[quadsii++] = xx;
			sprite_vertices[quadsii++] = yy;
			// sprite_vertices[quadsii++] = 0;
			// sprite_vertices[quadsii++] = 1;
			// sprite_vertices[quadsii++] = 0;

			// quadsi += 30;
			quadsi += SPRITE_MESH_SIZE;
			quads++;
		}
	}
	return quads;
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

	// GLint u_image = glGetUniformLocation( sp, "u_image" );
	// GLuint tex;
	// load_texture( "dude.png", &tex, 0 );
	// glUniform1i( u_image, 0 );

	setup_sprites();

	// TODO: needed later when using interleaved data (pos+uv+color?)
	// int fsize = sizeof( GLfloat );
	// int stride = fsize * 5;

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	GLuint vp_vbo;
	glGenBuffers( 1, &vp_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vp_vbo );

	glEnableVertexAttribArray( 0 );
	// glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, stride, NULL );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );

	// glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, (void*) 12);
	// glEnableVertexAttribArray( 1 );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	while ( !glfwWindowShouldClose( window ) ) {
		int w, h;
		glfwGetFramebufferSize( window, &w, &h );
		glViewport( 0, 0, w, h );

		glClear( GL_COLOR_BUFFER_BIT );

		// update_sprites();
		int quads = draw_sprites();

		glBufferData( GL_ARRAY_BUFFER,
				MAX_SPRITES * SPRITE_MESH_SIZE * sizeof( GLfloat ),
				sprite_vertices, GL_DYNAMIC_DRAW );
		glDrawArrays( GL_TRIANGLES, 0, quads * 6 );

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	glfwTerminate();
	return 0;
}

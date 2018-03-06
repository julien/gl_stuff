#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

struct sprite {
	float ax;
	float ay;
	float life;
	float rotation;
	float size;
	float vx;
	float vy;
	float x;
	float y;
};

int g_viewport_width = 1024;
int g_viewport_height = 768;
const unsigned int MAX_SPRITES = 4000;
const int SPRITE_MESH_SIZE = 12;
const int SPRITE_SIZE = 3;
const int SPRITE_VERTEX_SIZE = 12;
unsigned int free_sprite = 0;
unsigned int sprite_count = 0;
sprite sprites[MAX_SPRITES];
GLfloat vpos_data[MAX_SPRITES * SPRITE_MESH_SIZE];

GLfloat view_matrix[16] = {
	2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 0.0f
};

const int MAX_STACK = 100;
const int MAT_SIZE = 6;
const int MAT_STACK_SIZE = MAX_STACK * MAT_SIZE;
GLfloat mat[] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
GLfloat stack[MAX_STACK];
int stackp = 0;

void push_matrix() {
	stack[stackp + 0] = mat[0];
	stack[stackp + 1] = mat[1];
	stack[stackp + 2] = mat[2];
	stack[stackp + 3] = mat[3];
	stack[stackp + 4] = mat[4];
	stack[stackp + 5] = mat[5];
	stackp += 6;
}

void pop_matrix() {
	stackp -= 6;
	mat[0] = stack[stackp + 0];
	mat[1] = stack[stackp + 1];
	mat[2] = stack[stackp + 2];
	mat[3] = stack[stackp + 3];
	mat[4] = stack[stackp + 4];
	mat[5] = stack[stackp + 5];
}

void trans( float x, float y ) {
	mat[4] = mat[0] * x + mat[2] * y + mat[4];
	mat[5] = mat[1] * x + mat[3] * y + mat[5];
}

void scale( float x, float y ) {
	mat[0] = mat[0] * x;
	mat[1] = mat[1] * x;
	mat[2] = mat[2] * y;
	mat[3] = mat[3] * y;
}

void rot( float r ) {
	float a = mat[0];
	float b = mat[1];
	float c = mat[2];
	float d = mat[3];
	float sr = sinf(r);
	float cr = cosf(r);

	mat[0] = a * cr + c * sr;
	mat[1] = b * cr + d * sr;
	mat[2] = a * -sr + c * cr;
	mat[3] = b * -sr + d * cr;
}

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
		s->rotation = rand_range( 1, 9 ) * 0.1;
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

void drawRect(float x, float y, float w, float h) {
	float a = mat[0];
	float b = mat[1];
	float c = mat[2];
	float d = mat[3];
	float e = mat[4];
	float f = mat[5];
	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;
	float x2 = x;
	float y2 = y + h;
	float x3 = x;
	float y3 = y;
	float x4 = x + w;
	float y4 = y;
	float x5 = x + w;
	float y5 = y + h;
	int offset = 0;

	offset = sprite_count * SPRITE_VERTEX_SIZE;

	vpos_data[offset++] = x0 * a + y0 * c + e;
	vpos_data[offset++] = x0 * b + y0 * d + f;


	vpos_data[offset++] = x1 * a + y1 * c + e;
	vpos_data[offset++] = x1 * b + y1 * d + f;

	vpos_data[offset++] = x2 * a + y2 * c + e;
	vpos_data[offset++] = x2 * b + y2 * d + f;

	vpos_data[offset++] = x3 * a + y3 * c + e;
	vpos_data[offset++] = x3 * b + y3 * d + f;

	vpos_data[offset++] = x4 * a + y4 * c + e;
	vpos_data[offset++] = x4 * b + y4 * d + f;

	vpos_data[offset++] = x5 * a + y5 * c + e;
	vpos_data[offset++] = x5 * b + y5 * d + f;

	if ( ++sprite_count >= MAX_SPRITES ) {
		glBufferData( GL_ARRAY_BUFFER,
				sprite_count * SPRITE_MESH_SIZE * sizeof (GLfloat ),
				vpos_data, GL_DYNAMIC_DRAW );
		glDrawArrays( GL_TRIANGLES, 0, sprite_count * 6 );
		sprite_count = 0;
	}
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
		// static double previous_seconds = glfwGetTime();
		// double current_seconds = glfwGetTime();
		// double elapsed_seconds = current_seconds - previous_seconds;
		// previous_seconds = current_seconds;

		int w, h;
		glfwGetFramebufferSize( window, &w, &h );
		glViewport( 0, 0, w, h );

		glClear( GL_COLOR_BUFFER_BIT );

		for ( size_t i = 0; i < MAX_SPRITES; i++ ) {
			if ( sprites[i].life > 0 ) {

				push_matrix();

				trans( sprites[i].x, sprites[i].y );
				rot( sprites[i].rotation );
				sprites[i].rotation += 0.01;
				drawRect( sprites[i].x, sprites[i].y, sprites[i].size, sprites[i].size );

				pop_matrix();
			}
		}

		glBufferSubData( GL_ARRAY_BUFFER, 0,
				sprite_count * SPRITE_MESH_SIZE * sizeof (GLfloat ),
				vpos_data);
		glDrawArrays( GL_TRIANGLES, 0, sprite_count * 6 );

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	glfwTerminate();
	return 0;
}

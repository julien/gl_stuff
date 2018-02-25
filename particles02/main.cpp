#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

typedef struct vec2 {
	float x;
	float y;
} vec2;

typedef struct particle {
	struct vec2 pos;
	struct vec2 vel;
	struct vec2 acc;
	struct vec2 size;
	float life;
} particle;

int g_viewport_width = 1024;
int g_viewport_height = 768;

const unsigned int max_particles = 50000;
unsigned int free_particle = 0;
particle particles[max_particles];
const int particle_mesh_size = 30;
const int particle_uv_size = 10;

GLfloat mesh_vertices[max_particles * particle_mesh_size];

GLfloat view_matrix[] = {
	2.0f, 0.0f, 0.0f, -1.0f,
	0.0f, 2.0f, 0.0f,  1.0f,
	0.0f, 0.0f, 1.0f,  0.0f,
	0.0f, 0.0f, 0.0f,  1.0f,
};

GLint u_matrix = -1;

void setup_particles() {
	for ( unsigned int i = 0; i < max_particles; i++ ) {
		particle *p = &( particles[i] );
		float x = (g_viewport_width * 0.5) + rand_range( -100, 100 );
		float y = (g_viewport_height * 0.5) + rand_range( -100, 100 );

		p->pos.x = x;
		p->pos.y = y;

		p->size.x = particle_uv_size;
		p->size.y = particle_uv_size;

		p->life = -1.0;

		p->vel.x = 0;
		p->vel.y = 0;
		p->acc.x = 0;
		p->acc.y = 0;
	}
}

int find_free_particle() {
	for (unsigned int i = free_particle; i < max_particles; i++) {
		if (particles[i].life < 0) {
			free_particle = i;
			return i;
		}
	}
	for (unsigned int i = 0; i < free_particle; i++) {
		if (particles[i].life < 0) {
			free_particle = i;
			return i;
		}
	}
	return -1;
}

void draw_particles() {
	int quads = 0;
	int quads30 = 0;

	for ( unsigned int i = 0; i < max_particles; i++ ) {
		particle *p = &(particles[i]);

		if (p->life > 0) {

			int quads30i = quads30;
			float x = p->pos.x;
			float y = p->pos.y;
			float xx = x + p->size.x;
			float yy = y + p->size.y;

			mesh_vertices[quads30i++] = x;
			mesh_vertices[quads30i++] = y;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 1;

			mesh_vertices[quads30i++] = xx;
			mesh_vertices[quads30i++] = y;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 1;
			mesh_vertices[quads30i++] = 1;

			mesh_vertices[quads30i++] = x;
			mesh_vertices[quads30i++] = yy;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 0;

			mesh_vertices[quads30i++] = x;
			mesh_vertices[quads30i++] = yy;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 0;

			mesh_vertices[quads30i++] = xx;
			mesh_vertices[quads30i++] = y;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 1;
			mesh_vertices[quads30i++] = 1;

			mesh_vertices[quads30i++] = xx;
			mesh_vertices[quads30i++] = yy;
			mesh_vertices[quads30i++] = 0;
			mesh_vertices[quads30i++] = 1;
			mesh_vertices[quads30i++] = 0;

			quads30 += 30;
			quads++;
		}
	}

	glBufferData(
			GL_ARRAY_BUFFER,
			max_particles * 30 * sizeof( GLfloat ),
			mesh_vertices,
			GL_DYNAMIC_DRAW );
	glDrawArrays( GL_TRIANGLES, 0, quads * 6 );
}

GLuint create_shaders() {
	const char *vs_str = "#version 410\n"
		"layout (location=0) in vec3 a_position;"
		"layout (location=1) in vec2 a_texcoord;"
		"out vec2 v_texcoord;"
		"uniform mat4 u_matrix;"
		"void main() {"
		"  vec4 pos = vec4(a_position.x, a_position.y, a_position.z, 1.0);"
		"  v_texcoord = a_texcoord;"
		"  gl_Position = pos * u_matrix;"
		"}";
	const char *fs_str = "#version 410\n"
		"uniform sampler2D u_image;"
		"in vec2 v_texcoord;"
		"out vec4 frag_color;"
		"void main() {"
		"  frag_color = texture(u_image, v_texcoord);"
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

	setup_particles();

	GLuint sp = create_shaders();
	glUseProgram( sp );

	u_matrix = glGetUniformLocation( sp, "u_matrix" );
	GLint u_image = glGetUniformLocation( sp, "u_image" );

	view_matrix[0] *= 1.0f / (float) g_viewport_width;
	view_matrix[5] *= -1.0f / (float) g_viewport_height;

	glUniformMatrix4fv( u_matrix, 1, GL_FALSE, view_matrix );

	GLuint tex;
	load_texture( "alien.png", &tex, 0 );
	glUniform1i( u_image, 0 );

	GLuint vp_vbo;
	glGenBuffers( 1, &vp_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vp_vbo );

	int fsize = sizeof( GLfloat );
	int stride = fsize * 5;

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride, NULL );
	glEnableVertexAttribArray( 0 );

	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, (void*) 12);
	glEnableVertexAttribArray( 1 );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	while ( !glfwWindowShouldClose( window ) ) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		int newparticles = (int) (elapsed_seconds * 10000.0);
		if ( newparticles >= (int) (0.016f*1000.0) )
			newparticles = (int) (0.016f * 1000.0);

		for ( int i = 0; i < newparticles; i++ ) {
			int index = find_free_particle();

			if ( index == -1 ) {
				continue;
			}

			float vx = rand_range( -10, 10 );
			float vy = rand_range( -10, 10 );

			float x = (g_viewport_width * 0.5) + rand_range( -10, 10 );
			float y = (g_viewport_height * 0.5) + rand_range( -10, 10 );

			particles[index].pos.x = x;
			particles[index].pos.y = y;
			particles[index].acc.x = 0;
			particles[index].acc.y = 0;
			particles[index].vel.x = vx;
			particles[index].vel.y = vy;
			particles[index].life = 100.0f;
		}

		for ( unsigned int i = 0; i < max_particles; i++ ) {
			particle *p = &(particles[i]);

			if (p->life > 0.0f) {
				float delta = 0.05;
				float grav = 2.0;

				p->acc.y += grav;

				p->acc.x *= delta;
				p->acc.y *= delta;

				p->vel.x += p->acc.x;
				p->vel.y += p->acc.y;

				p->pos.x += p->vel.x;
				p->pos.y += p->vel.y;

				p->acc.x = 0.0;
				p->acc.y = 0.0;

				p->life--;
			} else {
				p->life = -1;
			}
		}

		int w, h;
		glfwGetFramebufferSize( window, &w, &h );
		glViewport( 0, 0, w, h );

		glClear( GL_COLOR_BUFFER_BIT );

		draw_particles();

		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( window, 1 );
		}
		glfwSwapBuffers( window );
	}

	glfwTerminate();
	return 0;
}

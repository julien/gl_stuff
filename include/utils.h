#ifndef __UTILS_H__
#define __UTILS_H__

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <math.h>
#include <stdio.h>
#include <time.h>

extern int g_viewport_width;
extern int g_viewport_height;

float rand_range( float min, float max ) {
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void print_infolog(GLuint index) {
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];

	glGetShaderInfoLog(index, max_length, &actual_length, log);
	fprintf( stdout, "%u:\n%s\n", index, log );
}

void glfw_framebuffer_size_callback( GLFWwindow *window, int width, int height ) {
	g_viewport_width = width;
	g_viewport_height = height;
	glViewport( 0, 0, g_viewport_width, g_viewport_height );
}

int load_texture( const char *file_name, GLuint *tex, int check ) {
	int x, y, n;
	int force_channels = 4;
	unsigned char *image_data = stbi_load( file_name, &x, &y, &n, force_channels );
	if ( !image_data ) {
		fprintf( stderr, "ERROR: could not load %s\n", file_name );
		return 0;
	}

	// NPOT check
	if ( ( check == 1 ) && ( ( x & ( x - 1 ) ) != 0 || ( y & ( y - 1 ) ) != 0 ) ) {
		fprintf( stderr, "WARNING: texture %s is not power-of-2 dimensions\n",
						 file_name );
	}

	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for ( int row = 0; row < half_height; row++ ) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + ( y - row - 1 ) * width_in_bytes;
		for ( int col = 0; col < width_in_bytes; col++ ) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	glGenTextures( 1, tex );
	glActiveTexture( GL_TEXTURE0 );

	glBindTexture( GL_TEXTURE_2D, *tex );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
								image_data );
	glGenerateMipmap( GL_TEXTURE_2D );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	GLfloat max_aniso = 0.0f;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso );

	stbi_image_free( image_data );

	return 1;
}
#endif

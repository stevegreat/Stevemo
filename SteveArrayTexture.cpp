#include "SteveArrayTexture.h"
#include <stdlib.h>
#include <sb6ktx.h>

static unsigned int seed = 0x13371337;

static inline float random_float() {
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ ( seed >> 4 ) ^ ( seed << 15 );

	*((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

	return ( res - 1.0f );
}

GLuint compile_shaders();

void SteveArrayTexture::startup() {
	vs_source = NULL;
	fs_source = NULL;

	rendering_program = compile_shaders( "array_texture_vs.txt", "array_texture_fs.txt" );
	
	glGenVertexArrays( 1, &vertex_array_object );
	glBindVertexArray( vertex_array_object );

	tex_alien_array = sb6::ktx::file::load( "media/textures/aliens.ktx" );
	glBindTexture( GL_TEXTURE_2D_ARRAY, tex_alien_array );
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	glGenBuffers( 1, &rain_buffer );
	glBindBuffer( GL_UNIFORM_BUFFER, rain_buffer );
	glBufferData( GL_UNIFORM_BUFFER, 256 * sizeof( vmath::vec4 ), NULL, GL_DYNAMIC_DRAW );

	for ( int i = 0; i < 256; ++i ) {
		droplet_x_offset[i] = random_float() * 2.0f - 1.0f;
		droplet_rot_speed[i] = ( random_float() + 0.5f ) * ( ( i & 1 ) ? -3.0f : 3.0f );
		droplet_fall_speed[i] = random_float() + 0.2f;
	}

	glBindVertexArray( vertex_array_object );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
	
void SteveArrayTexture::render( double currentTime ) {
	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float t  = (float) currentTime;

	glViewport( 0, 0, info.windowWidth, info.windowHeight );
	glClearBufferfv(GL_COLOR, 0, black);

	glUseProgram( rendering_program );

	glBindBufferBase( GL_UNIFORM_BUFFER, 0, rain_buffer );

	vmath::vec4 * droplet = (vmath::vec4 *) glMapBufferRange( GL_UNIFORM_BUFFER, 0, 256 * sizeof( vmath::vec4 ), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );

	for ( int i = 0; i < 256; ++i ) {
		droplet[i][0] = droplet_x_offset[i];
		droplet[i][1] = 2.0f - fmodf( ( t + float(i) ) * droplet_fall_speed[i], 4.31f );
		droplet[i][2] = t * droplet_rot_speed[i];
		droplet[i][3] = 0.0f;
	}

	glUnmapBuffer( GL_UNIFORM_BUFFER );

	for ( int i = 0; i < 256; ++i ) {
		glVertexAttribI1i( 0, i );
		glDrawArrays( GL_TRIANGLE_STRIP, 0 , 4 );
	}
}

void SteveArrayTexture::shutdown() {
}
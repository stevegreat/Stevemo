#include "SteveTunnelMipmap.h"
#include <sb6ktx.h>
#include <vmath.h>

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

void SteveTunnelMipmap::startup() {
	vs_source = 0;
	fs_source = 0;
	rendering_program = compile_shaders( "tunnel_vs.txt", "tunnel_fs.txt" );

	uniforms.mvp = glGetUniformLocation( rendering_program, "mvp" );
	uniforms.offset = glGetUniformLocation( rendering_program, "offset" );

	glGenVertexArrays( 1, &vertex_array_object );
	glBindVertexArray( vertex_array_object );

	tex_wall = sb6::ktx::file::load("media/textures/brick.ktx");
    tex_ceiling = sb6::ktx::file::load("media/textures/ceiling.ktx");
    tex_floor = sb6::ktx::file::load("media/textures/floor.ktx");

	int i;
	GLuint textures[] = { tex_floor, tex_ceiling, tex_wall };

	for ( i=0; i < 3; ++i ) {
		glBindTexture( GL_TEXTURE_2D, textures[i]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 10 );
	}

	glBindVertexArray( vertex_array_object );
}
	
void SteveTunnelMipmap::render( double currentTime ) {
	const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float time = (float) currentTime;
	
	glViewport( 0, 0, info.windowWidth, info.windowHeight );
	glClearBufferfv( GL_COLOR, 0, black );

	glUseProgram( rendering_program );

	vmath::mat4 proj_matrix = vmath::perspective(60.0f,
                                                (float)info.windowWidth / (float)info.windowHeight,
                                                0.1f, 100.0f);
	//sets a uniform!  a single float!
	glUniform1f( uniforms.offset, time * 0.03f );

	GLuint textures[] = { tex_wall, tex_floor, tex_wall, tex_ceiling };

	//random mipmap craziness
	for ( int i=0; i < 4; ++i ) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, (unsigned int)(time * 30.0f) % 10  );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  (unsigned int)(time * 30.0f) % 10  );
	}

	for ( int i=0; i < 4; ++i ) {
		vmath::mat4 mv_matrix = 
			vmath::rotate( 90.0f * (float)i, vmath::vec3( 0.0f, 0.0f, 1.0f ) ) *
			vmath::translate( -0.5f, 0.0f, -10.f ) *
			vmath::rotate( 90.f, 0.0f, 1.0f, 0.0f ) *
			vmath::scale( 30.0f, 1.0f, 1.0f );
		
		vmath::mat4 mvp = proj_matrix * mv_matrix;

		glUniformMatrix4fv( uniforms.mvp, 1, GL_FALSE, mvp );

		glBindTexture(GL_TEXTURE_2D, textures[i]);

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	}
}

void SteveTunnelMipmap::shutdown() {
	if ( vs_source != 0) {
		delete[] vs_source;
	}

	if ( fs_source != 0) {
		delete[] fs_source;
	}
	glDeleteVertexArrays( 1, &vertex_array_object );
	glDeleteProgram( rendering_program );
}
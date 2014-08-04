#include "SteveFirstApp.h"
#include <fstream>

GLuint compile_shaders();

void SteveFirstApp::startup() {
	vs_source = 0;
	fs_source = 0;
	rendering_program = compile_shaders( "vertex_shader.txt", "fragment_shader.txt" );
	glGenVertexArrays( 1, &vertex_array_object );
	glBindVertexArray( vertex_array_object );
}
	
void SteveFirstApp::render( double currentTime ) {
	const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f, 
		(float)cos(currentTime) * 0.5f + 0.5f, 
		0.0f, 1.0f };
	glClearBufferfv( GL_COLOR, 0, color );

	glUseProgram( rendering_program );
	glPointSize( 40.0f );
	glDrawArrays( GL_TRIANGLES, 0, 3 );
}

void SteveFirstApp::shutdown() {
	if ( vs_source != 0) {
		delete[] vs_source;
	}

	if ( fs_source != 0) {
		delete[] fs_source;
	}
	glDeleteVertexArrays( 1, &vertex_array_object );
	glDeleteProgram( rendering_program );
}
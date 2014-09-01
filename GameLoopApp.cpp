#include "GameLoopApp.h"
#include "TempPractice.h"
#include "XMLLevelLoader.h"
#include "Level.h"
#include <fstream>

void GameLoopApp::init()
{
    static const char title[] = "GAMEY";

    sb6::application::init();

    memcpy(info.title, title, sizeof(title));
}

size_t GameLoopApp::load_file( char* pszFilePath, GLchar* source ) {
	std::ifstream file;
	int i = -1;
	file.open( pszFilePath );

	while ( file.good() ) {
		source[++i] = file.get();
	}
	source[i] = '\0';
	return i;
}

//Compiling Shaders
GLuint GameLoopApp::compile_shaders( const char* vs_file_path, const char* fs_file_path ) {
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;
	vs_source = (GLchar*) new char[10000];
	fs_source = (GLchar*) new char[10000];

	if ( !this->load_file( (char*)vs_file_path, vs_source ) ) {
		//errors
	}

	if ( !this->load_file( (char*)fs_file_path, fs_source ) ) {
		//errors
	}

	program = glCreateProgram();
	vertex_shader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertex_shader, 1, (const GLchar **)&vs_source, NULL );
	glCompileShader( vertex_shader );

	fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragment_shader, 1, (const GLchar **)&fs_source, NULL );
	glCompileShader( fragment_shader );

	
	glAttachShader( program, vertex_shader );
	glAttachShader( program, fragment_shader );
	glLinkProgram( program );

	glDeleteShader( vertex_shader );
	glDeleteShader( fragment_shader );
	
	return program;
}
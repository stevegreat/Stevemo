#pragma once

#include "sb6.h"
#define 	GLFW_KEY_W   87
#define 	GLFW_KEY_Q   81
#define 	GLFW_KEY_E   69
#define 	GLFW_KEY_S   83
#define		GLFW_KEY_D	 68
#define		GLFW_KEY_A	 65


class GameLoopApp : public sb6::application {
protected:
	GLuint rendering_program;
	GLuint vertex_array_object;
	GLchar *vs_source, *fs_source;
	size_t load_file( char* pszFilePath, GLchar* source );
	GLuint compile_shaders( const char* vs_file_path, const char* fs_file_path );
public:
	void init();
	virtual void startup() = 0;
	virtual void render( double currentTime ) = 0;
	virtual void shutdown() = 0;
};
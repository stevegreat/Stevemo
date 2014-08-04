#pragma once

#include "GameLoopApp.h"

class SteveTunnelMipmap : public GameLoopApp {
public:
	void startup();
	void render( double currentTime );
	void shutdown();
protected:
	struct
    {
        GLint       mvp;
        GLint       offset;
    } uniforms;

	GLuint tex_wall;
	GLuint tex_ceiling;
	GLuint tex_floor;
};
#pragma once

#include <vmath.h>
#include "GameLoopApp.h"

class SteveUniformPractice : public GameLoopApp {
private:
    GLuint          vao;
    GLuint          buffer;
    GLint           mv_location;
    GLint           proj_location;

    float           aspect;
    vmath::mat4     proj_matrix;

public:
	void startup();
	void render( double currentTime );
	void onResize( int w, int h );
	void shutdown();
};
#pragma once

#include <vmath.h>
#include "GameLoopApp.h"

class SteveArrayTexture : public GameLoopApp {
private:
    GLuint          vao;
    GLuint          buffer;
    GLint           mv_location;
    GLint           proj_location;

    float           aspect;
    vmath::mat4     proj_matrix;

	GLuint          tex_alien_array;
    GLuint          rain_buffer;

    float           droplet_x_offset[256];
    float           droplet_rot_speed[256];
    float           droplet_fall_speed[256];

public:
	void startup();
	void render( double currentTime );
	void shutdown();
};
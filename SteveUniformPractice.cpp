#include "SteveUniformPractice.h"
#include <stdlib.h>

GLuint compile_shaders();

void SteveUniformPractice::startup() {
	vs_source = 0;
	fs_source = 0;
	rendering_program = compile_shaders( "uniform_vs.txt", "uniform_fs.txt" );
	glGenVertexArrays( 1, &vertex_array_object );
	glBindVertexArray( vertex_array_object );

    mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
    proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

        static const GLfloat vertex_positions[] =
        {
            -0.25f,  0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f,  0.25f,

            -0.25f,  0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
             0.25f,  0.25f,  0.25f,

             0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f, -0.25f
        };

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(vertex_positions),
                     vertex_positions,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
}
	
void SteveUniformPractice::render( double currentTime ) {
	 static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
    static const GLfloat one = 1.0f;

    glViewport(0, 0, info.windowWidth, info.windowHeight);
    glClearBufferfv(GL_COLOR, 0, green);
    glClearBufferfv(GL_DEPTH, 0, &one);

    glUseProgram(rendering_program);
    glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);


	for ( int i = 0; i < 24; ++i ) {
		float f = (float)i + (float)currentTime * 0.3f;
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
								vmath::translate(sinf(2.1f * f) * 2.0f,
													cosf(1.7f * f) * 2.0f,
													sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
								vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
								vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void SteveUniformPractice::onResize( int w, int h ) {
	sb6::application::onResize(w, h);

    aspect = (float)w / (float)h;
    proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
}

void SteveUniformPractice::shutdown() {
	glDeleteVertexArrays( 1, &vertex_array_object );
	glDeleteProgram( rendering_program );
	glDeleteBuffers( 1, &buffer );
}
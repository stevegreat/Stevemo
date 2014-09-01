#pragma once

#include "GameLoopApp.h"
#include "Level.h"
#include <vmath.h>


class SteveFogShader : public GameLoopApp {
public:
	void startup();
	void render( double currentTime );
	void shutdown();
	void onKey( int key, int action );
	void onMouseMove(int x, int y);
protected:
	Level			m_level;
	GLuint          buffer, position_buffer;
	GLfloat			m_offset, m_side_offset, m_AngleX, m_AngleY;
	bool			m_forward_press, m_backward_press, m_left_press, m_right_press, m_rotate_right_press, m_rotate_left_press;
	int				m_MouseX, m_MouseY;

	vmath::vec3 m_eye_position, m_box_position;
	vmath::vec3 m_eye_direction, m_box_direction;
	vmath::vec3 m_eye_up, m_box_up;
	GLfloat m_angle;
	GLfloat m_angleY;

	struct
    {
        GLint       mvp;
        GLint       offset;
		GLint		eye;
		GLint		gvp;
    } uniforms;
};
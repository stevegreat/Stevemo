#include "SteveFogShader.h"
#include "XMLLevelLoader.h"
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

void SteveFogShader::startup() {
	vs_source = 0;
	fs_source = 0;
	m_offset = m_side_offset = 0.0f;
	m_forward_press = m_backward_press = m_right_press = m_left_press = m_rotate_right_press = m_rotate_left_press = false;
	m_MouseX = m_MouseY = 0;
	m_angle = 3*M_PI/2;
	m_angleY = 0.0f;

	m_eye_position = vmath::vec3( 0.0f, 0.0f, 0.0f );
	m_eye_direction = vmath::vec3( 0.0f, 0.0f, -1.0f );
	m_eye_up = vmath::vec3( 0.0f, 1.0f, 0.0f );

	m_box_position = vmath::vec3( 0.0f, 0.0f, 20.0f ); 
	m_box_direction = vmath::vec3( 0.0f, 0.0f, -1.0f );
	m_box_up = vmath::vec3( 0.0f, 1.0f, 0.0f );

	onMouseMove( 1, 0 );

	// Load up the level
	XMLLevelLoader xml_level;
	xml_level.load_xml( "media/levels/demo_level.oel" );
	xml_level.build_level( m_level );
	xml_level.unload_xml();

	rendering_program = compile_shaders( "fog_shader_vs.txt", "fog_shader_fs.txt" );

	static const GLfloat vertex_positions[] =
        {
            -1.f, -1.f,  1.f,	 1.f, -1.f,  1.f,	-1.f,  1.f,  1.f,	 1.f,  1.f,  1.f,  // ABCD
			 1.f, -1.f,  1.f,	 1.f, -1.f, -1.f,	 1.f,  1.f,  1.f,	 1.f,  1.f, -1.f,  // BFDH
			 1.f, -1.f, -1.f,	-1.f, -1.f, -1.f,	 1.f,  1.f, -1.f,	-1.f,  1.f, -1.f,  // FEHG
			-1.f, -1.f, -1.f,	-1.f, -1.f,  1.f,	-1.f,  1.f, -1.f,	-1.f,  1.f,  1.f,  // EAGC
			-1.f, -1.f, -1.f,	 1.f, -1.f, -1.f,	-1.f, -1.f,  1.f,	 1.f, -1.f,  1.f,  // EFAB
			-1.f,  1.f,  1.f,	 1.f,  1.f,  1.f,	-1.f,  1.f, -1.f,	 1.f,  1.f, -1.f   // CDGH
        }; // ABCD BFDH FEHG EAGC EFAB CDGH

	static const GLfloat vertex_position_marker[] =
		{
			0.0f, 2.0f, 0.0f,
			0.0f, 0.0f, 4.0f,
			2.0f, 0.0f, 0.0f,
			3.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 3.0f,
			3.0f, 0.0f, 0.0f
		};
	uniforms.mvp = glGetUniformLocation( rendering_program, "mvp" );
	uniforms.offset = glGetUniformLocation( rendering_program, "offset" );

	glGenBuffers(1, &buffer);
	glGenBuffers(1, &position_buffer );
	
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER,
		sizeof( vertex_positions ),
		vertex_positions,
		GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, position_buffer );
	glBufferData( GL_ARRAY_BUFFER,
		sizeof( vertex_position_marker ),
		vertex_position_marker,
		GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
}
	
void SteveFogShader::render( double currentTime ) {
	const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const GLfloat speed = 0.01f;
	float time = (float) currentTime;

	vmath::vec3 eye_right = vmath::normalize( vmath::cross( m_eye_direction, m_eye_up ) );
	vmath::vec3 eye_up = vmath::cross( eye_right, m_eye_direction );

	if ( m_forward_press ) {
		m_eye_position += m_eye_direction*speed;
	} else if ( m_backward_press ) {
		m_eye_position -= m_eye_direction*speed;
	}

	if ( m_left_press ) {
		m_eye_position -= eye_right*speed;
	} else if ( m_right_press ) {
		m_eye_position += eye_right*speed;
	}
	
	glViewport( 0, 0, info.windowWidth, info.windowHeight );
	glClearBufferfv( GL_COLOR, 0, black );

	glUseProgram( rendering_program );

	vmath::mat4 proj_matrix = vmath::perspective(60.0f,
                                                (float)info.windowWidth / (float)info.windowHeight,
                                                0.1f, 1000.0f);
	vmath::mat4 box_spin_matrix =
		vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

		// rotating of object
	vmath::mat4 RotationMat = vmath::mat4( 
		vmath::vec4( eye_right[0], eye_right[1], eye_right[2], 0.0f), 
		vmath::vec4( eye_up[0], eye_up[1], eye_up[2], 0.0f), 
		vmath::vec4( -m_eye_direction[0], -m_eye_direction[1], -m_eye_direction[2], 0.0f), 
		vmath::vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
	
	vmath::mat4 mvp = proj_matrix  * RotationMat.transpose() * vmath::translate( -m_eye_position ) * vmath::translate( -m_box_position ) * box_spin_matrix;

	//sets a uniform!  a single float!
	glUniform1f( uniforms.offset, m_offset );
	glUniformMatrix4fv( uniforms.mvp, 1, GL_FALSE, mvp );
	glUniform3fv( uniforms.gvp, 1, m_box_position );
	glUniform3fv( uniforms.eye, 1, m_eye_position );

	glDrawArrays( GL_TRIANGLE_STRIP, 4, 4 );
	glDrawArrays( GL_TRIANGLE_STRIP, 8, 4 );
	glDrawArrays( GL_TRIANGLE_STRIP, 12, 4 );
	glDrawArrays( GL_TRIANGLE_STRIP, 16, 4 );
	glDrawArrays( GL_TRIANGLE_STRIP, 20, 4 );
	wchar_t  debugText[100];
	//swprintf(debugText, 100, L"EYE POSITION: %f %f %f ANGLE: %f\n", m_eye_position[0], m_eye_position[1], m_eye_position[2], m_angle );
	//OutputDebugString( debugText );
}

void SteveFogShader::shutdown() {
	if ( vs_source != 0) {
		delete[] vs_source;
	}

	if ( fs_source != 0) {
		delete[] fs_source;
	}
	
	glDeleteProgram( rendering_program );
	glDeleteBuffers( 1, &buffer );
}

void SteveFogShader::onKey( int key, int action ) {
	switch ( key ) {
	case GLFW_KEY_W:
		m_forward_press = ( GLFW_PRESS == action );
		break;
	case GLFW_KEY_S:
		m_backward_press = ( GLFW_PRESS == action );
		break;
	case GLFW_KEY_Q:
		m_left_press = ( GLFW_PRESS == action );
		break;
	case GLFW_KEY_E:
		m_right_press = ( GLFW_PRESS == action );
		break;
	case GLFW_KEY_D:
		m_rotate_right_press = ( GLFW_PRESS == action );
		break;
	case GLFW_KEY_A:
		m_rotate_left_press = ( GLFW_PRESS == action );
		break;
	}
}

void SteveFogShader::onMouseMove(int x, int y)
{
	const GLfloat rot_speed = 0.01f;
	int deltaX, deltaY;

	deltaX = m_MouseX - x;
	deltaY = m_MouseY - y;

	m_MouseX = x;
	m_MouseY = y;

	if ( y > info.windowHeight )
		m_MouseY = info.windowHeight;

	if ( deltaX < 0 ) {
		m_angle += rot_speed;
		m_eye_direction[0] = cos(m_angle);
		m_eye_direction[2] = sin(m_angle);
	} else if ( deltaX > 0 ) {
		m_angle -= rot_speed;
		m_eye_direction[0] = cos(m_angle);
		m_eye_direction[2] = sin(m_angle);
	}

	if ( deltaY < 0 ) {
		m_angleY += rot_speed;
	} else if ( deltaY > 0 ) {
		m_angleY -= rot_speed;
	}

	//m_eye_direction[0] = cos(m_angle);
	//m_eye_direction[1] = sin(m_angleY);
	//m_eye_direction[2] = sin(m_angle) + cos(m_angleY);
	//GLfloat mag = sqrt( m_eye_direction[0]*m_eye_direction[0] + m_eye_direction[1]*m_eye_direction[1] + m_eye_direction[2]*m_eye_direction[2] ); 
	//m_eye_direction[0] /= mag;
	//m_eye_direction[1] /= mag;
	//m_eye_direction[2] /= mag;
}
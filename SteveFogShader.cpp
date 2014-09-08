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

static vmath::vec4 entity_colors[] = { 
	vmath::vec4( 0.0f, 0.0f, 0.0f, 0.0f ),
	vmath::vec4( 0.0f, 1.0f, 0.0f, 1.0f ),
	vmath::vec4( 1.0f, 0.0f, 0.0f, 1.0f ),
	vmath::vec4( 0.2f, 0.0f, 1.0f, 1.0f ),
	vmath::vec4( 1.0f, 0.0f, 1.0f, 1.0f ) };

#define GREY 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define CYAN 5
#define MAGENTA 6
#define WHITE 7
static vmath::vec4 wall_colors[] = { 
	vmath::vec4( 0.55f, 0.55f, 0.55f, 1.0f ), // gray
	vmath::vec4( 1.0f, 0.0f, 0.0f, 1.0f ), // red
	vmath::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), // green
	vmath::vec4( 0.0f, 0.0f, 1.0f, 1.0f ), // blue
	vmath::vec4( 1.0f, 1.0f, 0.0f, 1.0f ), // yellow
	vmath::vec4( 0.0f, 1.0f, 1.0f, 1.0f ), // cyan
	vmath::vec4( 1.0f, 0.0f, 1.0f, 1.0f ), // magenta
	vmath::vec4( 1.0f, 1.0f, 1.0f, 1.0f )}; // white

static vmath::vec4 navigation_colors[] = {
	vmath::vec4( 1.0f, 0.0f, 0.0f, 1.0f ), // red is x
	vmath::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), // green is y
	vmath::vec4( 0.0f, 0.0f, 1.0f, 1.0f ), // blue is z
};

GLuint compile_shaders();

void SteveFogShader::startup() {
	vs_source = 0;
	fs_source = 0;
	m_offset = m_side_offset = 0.0f;
	m_forward_press = m_backward_press = m_right_press = m_left_press = m_rotate_right_press = m_rotate_left_press = false;
	m_fog = false;
	m_MouseX = m_MouseY = 0;
	m_angle = M_PI;
	m_angleY = 0.0f;

	m_eye_position = vmath::vec3( 0.0f, 0.0f, 0.0f );
	m_eye_direction = vmath::vec3( 0.0f, 0.0f, -1.0f );
	m_eye_up = vmath::vec3( 0.0f, 1.0f, 0.0f );

	m_box_position = vmath::vec3( 0.0f, 0.0f, 20.0f ); 
	m_box_direction = vmath::vec3( 0.0f, 0.0f, -1.0f );
	m_box_up = vmath::vec3( 0.0f, 1.0f, 0.0f );

	// Load up the level
	XMLLevelLoader xml_level;
	xml_level.load_xml( "media/levels/demo_level.oel" );
	//xml_level.load_xml( "media/levels/testing_things.oel" );
	xml_level.build_level( m_level );
	xml_level.unload_xml();

	int start = m_level.GetStartIndex();

	if ( start >= 0 )
		m_eye_position = vmath::vec3( m_level.GetEntityFloatX( start ), 0.0f, m_level.GetEntityFloatY( start ) );

	onMouseMove( 1, 0 );

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

	static const GLfloat walls[] = 
	{
		 8.0f, -8.0f,  8.0f,   8.0f, -8.0f, -8.0f,   8.0f, 8.0f,  8.0f,   8.0f, 8.0f, -8.0f, // EAST WALL  
		-8.0f, -8.0f, -8.0f,  -8.0f, -8.0f,  8.0f,  -8.0f, 8.0f, -8.0f,  -8.0f, 8.0f,  8.0f, // WEST WALL
		 8.0f, -8.0f, -8.0f,  -8.0f, -8.0f, -8.0f,   8.0f, 8.0f, -8.0f,  -8.0f, 8.0f, -8.0f, // NORTH WALL
		-8.0f, -8.0f,  8.0f,   8.0f, -8.0f,  8.0f,  -8.0f, 8.0f,  8.0f,   8.0f, 8.0f,  8.0f, // SOUTH WALL
		 8.0f, -8.0f,  8.0f,  -8.0f, -8.0f, -8.0f,   8.0f, 8.0f,  8.0f,  -8.0f, 8.0f, -8.0f, // SOUTH WEST WALL
		 8.0f, -8.0f, -8.0f,  -8.0f, -8.0f,  8.0f,   8.0f, 8.0f, -8.0f,  -8.0f, 8.0f,  8.0f, // SOUTH EAST WALL
		-8.0f, -8.0f,  8.0f,   8.0f, -8.0f, -8.0f,  -8.0f, 8.0f,  8.0f,   8.0f, 8.0f, -8.0f, // NORTH WEST WALL
		-8.0f, -8.0f, -8.0f,   8.0f, -8.0f,  8.0f,  -8.0f, 8.0f, -8.0f,   8.0f, 8.0f,  8.0f, // NORTH EAST WALL
	};

		// x, y, z
	static const GLfloat navigation_lines[] =
	{
		0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f
	};

	uniforms.mvp = glGetUniformLocation( rendering_program, "mvp" );
	uniforms.gvp = glGetUniformLocation( rendering_program, "gvp" );
	uniforms.offset = glGetUniformLocation( rendering_program, "offset" );
	uniforms.eye = glGetUniformLocation( rendering_program, "eye" );
	uniforms.gvp = glGetUniformLocation( rendering_program, "gvp" );
	uniforms.show_fog = glGetUniformLocation( rendering_program, "show_fog" );
	uniforms.main_color = glGetUniformLocation( rendering_program, "main_color" );

	glGenBuffers(1, &buffer);
	
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertex_positions ) + sizeof( walls ) + sizeof(navigation_lines), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertex_positions ), vertex_positions );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof( vertex_positions ), sizeof( walls ), walls );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof( vertex_positions ) + sizeof( walls ), sizeof( navigation_lines ), navigation_lines );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// Culling and Depth Test
	glEnable( GL_CULL_FACE );
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
	glCullFace( GL_BACK );
}
	
void SteveFogShader::render( double currentTime ) {
	const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const GLfloat speed = 0.03f;
	float time = (float) currentTime;

	static const GLfloat one = 1.0f;

	glClearBufferfv( GL_DEPTH, 0, &one );

	if ( m_forward_press ) {
		m_eye_position += m_eye_direction*speed;
	} else if ( m_backward_press ) {
		m_eye_position -= m_eye_direction*speed;
	}

	vmath::vec3 eye_right = vmath::normalize( vmath::cross( m_eye_direction, m_eye_up ) );

	if ( m_left_press ) {
		m_eye_position -= eye_right*speed;
	} else if ( m_right_press ) {
		m_eye_position += eye_right*speed;
	}

	wchar_t  debugText[200];
	swprintf(debugText, 200, L"EYE: %f %f %f EYE DIRECTION: %f %f %f ANGLE: %f\n", m_eye_position[0] / 16, m_eye_position[1], m_eye_position[2] /16 ,  m_eye_direction[0], m_eye_direction[1], m_eye_direction[2], m_angle );
	OutputDebugString( debugText );
	
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
	vmath::mat4 RotationMat = vmath::lookat( m_eye_position, m_eye_position + m_eye_direction*speed, m_eye_up );
	vmath::mat4 mvp = proj_matrix  * RotationMat;

	//sets a uniform!  a single float!
	glUniform1f( uniforms.offset, m_offset );
	glUniformMatrix4fv( uniforms.mvp, 1, GL_FALSE, mvp );
	glUniform3fv( uniforms.eye, 1, m_eye_position );
	glUniform1i( uniforms.show_fog, m_fog );

	vmath::mat4 gvp;

	// Render some helpful navigation
	gvp = vmath::translate( 0.0f, 0.0f, 0.0f );
	glUniformMatrix4fv( uniforms.gvp, 1, GL_FALSE, gvp );
	glUniform4fv( uniforms.main_color, 1, navigation_colors[ 0 ] );
	glDrawArrays( GL_LINES, 56, 2 );
	glUniform4fv( uniforms.main_color, 1, navigation_colors[ 1 ] );
	glDrawArrays( GL_LINES, 58, 2 );
	glUniform4fv( uniforms.main_color, 1, navigation_colors[ 2 ] );
	glDrawArrays( GL_LINES, 60, 2 );

	int count = m_level.GetEntityCount();
	for ( int i = 0; i < count; ++i ) {
		gvp = vmath::translate( m_level.GetEntityFloatX( i ), 0.0f, m_level.GetEntityFloatY( i ) ) * box_spin_matrix;
		glUniformMatrix4fv( uniforms.gvp, 1, GL_FALSE, gvp );
		glUniform4fv( uniforms.main_color, 1, entity_colors[ m_level.GetEntityType(i) ] );

		glDrawArrays( GL_TRIANGLE_STRIP, 4, 4 );
		glDrawArrays( GL_TRIANGLE_STRIP, 8, 4 );
		glDrawArrays( GL_TRIANGLE_STRIP, 12, 4 );
		glDrawArrays( GL_TRIANGLE_STRIP, 16, 4 );
		glDrawArrays( GL_TRIANGLE_STRIP, 20, 4 );
	} 

	int tile_type;
	for ( int i=0; i < MAX_GRID_ROW; ++i ) {
		for ( int j=0; j < MAX_GRID_COL; ++j ) {
			tile_type = m_level.GetTile( i, j );
			bool render = true;
			int position = 0;
			switch( tile_type ) {
			case EAST_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[GREY] );
				position = 24;
				break;
			case WEST_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[GREEN] );
				position = 28;
				break;
			case NORTH_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[BLUE] );
				position = 32;
				break;
			case SOUTH_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[RED] );
				position = 36;
				break;
			case SW_SLANT_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[YELLOW] );
				position = 40;
				break;
			case SE_SLANT_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[CYAN] );
				position = 44;
				break;
			case NW_SLANT_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[MAGENTA] );
				position = 48;
				break;
			case NE_SLANT_WALL:
				glUniform4fv( uniforms.main_color, 1, wall_colors[WHITE] );
				position = 52;
				break;
			default:
				render = false;
				break;
			}

			if ( render ) {
				gvp = vmath::translate( vmath::vec3( j*TILE_HEIGHT, 0.0f, i*TILE_WIDTH ) );
				glUniformMatrix4fv( uniforms.gvp, 1, GL_FALSE, gvp );
				glDrawArrays( GL_TRIANGLE_STRIP, position, 4 );
			}
		}
	}

	//wchar_t  debugText[100];
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
	case GLFW_KEY_F1: // turn on fog
		if( GLFW_PRESS == action )
			m_fog = !m_fog;
		break;
	}
}

void SteveFogShader::onMouseMove(int x, int y)
{
	const GLfloat rot_speed = 0.03f;
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
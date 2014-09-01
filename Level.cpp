#include "Level.h"

Level::Level() {
	for( int i=0; i < MAX_GRID_ROW; ++i ) {
		for ( int j=0; j < MAX_GRID_COL; ++j ) {
			m_tiles[ i ][ j ] = LevelTileType::NOTHING;
		}
	}

	m_level_entities_count = 0;
	m_start_index = -1;
	m_end_index = -1;
}

Level::~Level() {

}

int Level::SetTile( const int type, const int row, const int col ) {
	if ( row > MAX_GRID_ROW || row < 0 )
		return LevelError::TILE_OUT_OF_BOUNDS;

	if (col > MAX_GRID_COL || col < 0 )
		return LevelError::TILE_OUT_OF_BOUNDS;

	m_tiles[ row ][ col ] = type;

	return LevelError::LEVEL_SUCCESS;
}

int Level::AddEntity( const int type, const int x, const int y ) {
	const LevelEntity entity = { type, x, y };
	return AddEntity( entity );
}

int Level::AddEntity( const LevelEntity entity ) {
	if ( m_level_entities_count == MAX_LEVEL_ENTITIES )
		return LevelError::TOO_MANY_LEVEL_ENTITIES;

	if ( LevelEntityType::START == entity.type ) {
		if ( m_start_index > -1 )
			return LevelError::START_ALREADY_ASSIGNED;
		m_start_index = m_level_entities_count;
	}

	if ( LevelEntityType::END == entity.type ) {
		if ( m_end_index > -1 )
			return LevelError::END_ALREADY_ASSIGNED;
		m_end_index = m_level_entities_count ;
	}

	m_entities[ m_level_entities_count ] = entity;
	++m_level_entities_count;
	return LevelError::LEVEL_SUCCESS;
}
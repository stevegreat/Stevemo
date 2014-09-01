#include "XMLLevelLoader.h"
#include "Level.h"
#include <string> // std::string
#include <windows.h> // OutputDebugString

XMLLevelLoader::XMLLevelLoader() {
	m_pDoc = NULL;
}

XMLLevelLoader::~XMLLevelLoader() {
	this->unload_xml();
}

int XMLLevelLoader::load_xml( const char* str_file_name ) {
	m_pDoc = new tinyxml2::XMLDocument();
	tinyxml2::XMLError error = m_pDoc->LoadFile( str_file_name );
	bool failed = false;

	char str_debug_text[100];
	switch( error ) {
	case tinyxml2::XML_NO_ERROR:
		sprintf_s( str_debug_text, 100, "Level XML Loaded Successfully! :%s\n", str_file_name );
		break;
	case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
		sprintf_s( str_debug_text, 100, "Level has failed to load.  The file is not found :%s\n", str_file_name );
		failed = true;
		break;
	case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
		sprintf_s( str_debug_text, 100, "Level has failed to load.  The file could not open :%s\n", str_file_name );
		failed = true;
		break;
	case tinyxml2::XML_ERROR_FILE_READ_ERROR:
		sprintf_s( str_debug_text, 100, "Level has failed to load.  The file could not be read :%s\n", str_file_name );
		failed = true;
		break;
	default:
		sprintf_s( str_debug_text, 100, "Level has failed to load.  Unknown error :%s\n", str_file_name );
		failed = true;
		break;
	}

	OutputDebugStringA( str_debug_text );

	if ( failed )
		this->unload_xml();

	return error;
}

void XMLLevelLoader::unload_xml() {
	if ( m_pDoc ) {
		delete( m_pDoc ); m_pDoc = NULL;
	}
}

int XMLLevelLoader::build_level( Level& ref_level ) {
	if ( !m_pDoc ) {
		OutputDebugStringA( "Error when building level from XML.  XML has not been loaded or had an error.\n" );
		return XMLLevelError::XML_FAILED_TO_LOAD;
	}

	std::string szTiles, szTileRow, szTile;
	std::string szDelimiter = "\n";
	std::string szDelimiter2 = ",";
	tinyxml2::XMLElement* pElement = m_pDoc->FirstChildElement( "level" );

	if ( !pElement ) {
		OutputDebugStringA( "Error when building level from XML.  XML couldn't find the level node.\n" );
		return XMLLevelError::XML_FAILED_TO_LOAD;
	}

	pElement = pElement->FirstChildElement( "tiles" );

	if ( !pElement ) {
		OutputDebugStringA( "Error when building level from XML.  XML couldn't find the tile node.\n" );
		return XMLLevelError::XML_FAILED_TO_LOAD;
	}

	szTiles = pElement->FirstChild()->Value();
	
	size_t i = 0, j = 0;
	int tile = -1;
	int tile_row = 0, tile_col = 0;
	while ( ( i = szTiles.find( szDelimiter ) ) != std::string::npos  ) {
		szTileRow = szTiles.substr( 0, i );

		while( ( j = szTileRow.find( szDelimiter2 ) ) != std::string::npos ) { 
			szTile = szTileRow.substr( 0, j );
			szTileRow.erase( 0, j + szDelimiter2.length() );

			tile = atoi( szTile.c_str() );
			ref_level.SetTile( tile, tile_row, tile_col );
			++tile_col;
		}

		szTiles.erase( 0, i + szDelimiter.length() );
		tile_col = 0;

		++tile_row;
	}

	pElement = pElement->NextSiblingElement( "player_objects" );

	if ( !pElement ) {
		OutputDebugStringA( "Error when building level from XML.  XML couldn't find the player_objects node.\n" );
		return XMLLevelError::XML_FAILED_TO_LOAD;
	}

	pElement = pElement->FirstChildElement();

	do {
		switch ( ref_level.AddEntity( pElement->IntAttribute( "type" ), pElement->IntAttribute( "x" ), pElement->IntAttribute( "y" ) ) ) {
		case LevelError::LEVEL_SUCCESS:
			continue;
		case LevelError::START_ALREADY_ASSIGNED:
			OutputDebugStringA( "Error when building level from XML.  Start entity already assigned.  Consider revising the level.\n" );
			break;
		case LevelError::END_ALREADY_ASSIGNED:
			OutputDebugStringA( "Error when building level from XML.  End entity already assigned.  Consider revising the level.\n" );
			break;
		case LevelError::TOO_MANY_LEVEL_ENTITIES:
			OutputDebugStringA( "Error when building level from XML.  Too many level entities.  Consider revising the level.\n" );
			break;
		}
	} while ( pElement = pElement->NextSiblingElement() );

	OutputDebugStringA( "Level Built Successfully!\n" );
	return XMLLevelError::XML_SUCCESS;
}
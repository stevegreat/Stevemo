#define MAX_LEVEL_WIDTH 640
#define MAX_LEVEL_HEIGHT 480
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define MAX_GRID_ROW MAX_LEVEL_HEIGHT / TILE_HEIGHT
#define MAX_GRID_COL MAX_LEVEL_WIDTH / TILE_WIDTH
#define MAX_LEVEL_ENTITIES 50

enum LevelEntityType {
	START = 1,
	END,
	GOOD_PICKUP,
	BAD_PICKUP
};

// tile IDs exported from OGMO
enum LevelTileType {
	NOTHING = -1,
	WEST_WALL = 7,
	EAST_WALL = 15,
	NORTH_WALL = 13,
	SOUTH_WALL = 14,
	SW_SLANT_WALL = 5,
	NW_SLANT_WALL = 22,
	NE_SLANT_WALL = 21,
	SE_SLANT_WALL = 6
};

// Level error codes
enum LevelError {
	LEVEL_SUCCESS = 0,
	TILE_OUT_OF_BOUNDS,
	TOO_MANY_LEVEL_ENTITIES,
	START_ALREADY_ASSIGNED,
	END_ALREADY_ASSIGNED
};

struct LevelEntity{
	int type;
	int x;
	int y;
};

class Level {
private:
	int m_tiles[ MAX_GRID_ROW ][ MAX_GRID_COL ];
	LevelEntity m_entities[ MAX_LEVEL_ENTITIES ];
	int m_start_index;
	int m_end_index;
	int m_level_entities_count;
public:
	Level();
	~Level();
	int SetTile( const int type, const int row, const int col );
	int GetTile( const int row, const int col );
	int AddEntity( const int type, const int x, const int y );
	int AddEntity( const LevelEntity entity );
	int GetEntityCount();
	int GetEntityType( int i );
	int GetEntityX( int i );
	int GetEntityY( int i );
	float GetEntityFloatX( int i );
	float GetEntityFloatY( int i );
	int GetStartIndex();
};
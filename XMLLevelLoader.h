// XMLLoader ( implementation )

#include "XMLLoader.h"

class Level;

enum XMLLevelError {
	XML_SUCCESS = 0,
	XML_FAILED_TO_LOAD
};

class XMLLevelLoader : public XMLLoader {
public:
	XMLLevelLoader();
	~XMLLevelLoader();
	int load_xml( const char* str_file_name );
	void unload_xml();
	int build_level( Level& );
};
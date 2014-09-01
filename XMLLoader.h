// XMLLoader ( abstract )

#include "../tinyxml2/tinyxml2.h"

class XMLLoader {
protected:
	tinyxml2::XMLDocument* m_pDoc;
public:
	virtual int load_xml( const char* ) = 0;
	virtual void unload_xml() = 0;
};
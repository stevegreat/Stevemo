#pragma once

#include "GameLoopApp.h"

class SteveFirstApp : public GameLoopApp {
public:
	void startup();
	void render( double currentTime );
	void shutdown();
};
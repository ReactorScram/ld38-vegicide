#ifndef COLORADO_SCREEN_OPTIONS_H
#define COLORADO_SCREEN_OPTIONS_H

#include "colorado/dllspec.h"

namespace GetOpt {
class GetOpt_pp;
}

// Anything needed to initialize SDL
namespace Colorado {
struct COLORADO_DLLSPEC ScreenOptions {
public:
	int width, height;
	bool fullscreen;
	
	ScreenOptions ();
	
	void parse (GetOpt::GetOpt_pp &);
};
}

#endif // SCREENOPTIONS_H

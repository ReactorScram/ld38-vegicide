#include "colorado/screen-options.h"

//#include "getoptpp/getopt_pp.h"

namespace Colorado {
ScreenOptions::ScreenOptions () {
	width = 800;
	height = 480;
	fullscreen = false;
}
/*
void ScreenOptions::parse (GetOpt::GetOpt_pp & opts) {
	opts >> GetOpt::Option ("width", width);
	opts >> GetOpt::Option ("height", height);
	opts >> GetOpt::OptionPresent ("fullscreen", fullscreen);
}
*/
}

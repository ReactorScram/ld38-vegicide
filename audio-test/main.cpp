#include <unistd.h>
#include <string>

#include "terf/terf.h"
#include "audio/audio.h"

using namespace std;

int main () {
	const string rom_name = "vegicide-assets.tar";
	Terf::Archive terf (rom_name, rom_name + ".index");
	terf.enableTerfLookup = true;
	terf.enableFsLookup = true;
	
	Audio audio (terf);
	
	AudioFrame frame;
	frame.bgm = EMusic::Ambient;
	
	while (true) {
		audio.update (frame);
		usleep (1000 * 16);
	}
}

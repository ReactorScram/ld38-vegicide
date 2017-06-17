#include <unistd.h>
#include <string>

#include "terf/terf.h"
#include "audio/streamer.h"

using namespace std;

int main () {
	const string rom_name = "vegicide-assets.tar";
	Terf::Archive terf (rom_name, rom_name + ".index");
	terf.enableTerfLookup = true;
	terf.enableFsLookup = true;
	
	Streamer audio (terf);
	
	auto bgm = EMusic::Ambient;
	
	while (true) {
		audio.update (bgm);
		usleep (1000 * 16);
	}
}

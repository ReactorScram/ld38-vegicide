#include "emscripten.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

#include "platformer/animation.h"

#include "platformer/level.h"

using namespace std;

Level::Level (): width (0), height (0), data () {}

Level::Level (const vector <uint8_t> & bytes) {
	width = *(int16_t *)&bytes [0];
	height = *(int16_t *)&bytes [2];
	int size = width * height;
	
	data = vector <int16_t> ((int16_t *)&bytes [4], (int16_t *)&bytes [4 + 2 * size]);
}

extern "C" {
	typedef void * VgHandle;
	
	double animate (double t);
	char * get_msg (double t);
	VgHandle vegicide_init ();
}

char foo[] = "Foosball";
char bar[] = "Bartender";

double animate (double t) {
	return fmod (t, 60.0) / 60.0;
}

char * get_msg (double t) {
	t = animate (t);
	
	if (t > 0.5) {
		return foo;
	}
	else {
		return bar;
	}
}

vector <uint8_t> lookup_file (const std::string & fn) {
	ifstream file;
	file.open (fn, ios_base::in | ios_base::binary);
	
	if (! file.is_open ()) {
		return vector <uint8_t> ();
	}
	
	file.seekg (0, ios_base::end);
	
	size_t fileLength = file.tellg ();
	file.seekg (0, ios_base::beg);
	
	vector <uint8_t> buffer;
	buffer.resize (fileLength);
	
	file.read ((char *)buffer.data (), fileLength);
	
	file.close ();
	
	return buffer;
}

struct VgContext {
	Level level;
	
	VgContext () {
		emscripten_wget ("maps/demo.bin", "maps/demo.bin");
		
		level = Level (lookup_file ("maps/demo.bin"));
	}
};

VgHandle vegicide_init () {
	return new VgContext ();
}

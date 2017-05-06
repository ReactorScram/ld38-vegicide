#include "emscripten.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "platformer/animation.h"
#include "platformer/level.h"
#include "platformer/logic.h"
#include "platformer/scene-json.h"

// Don't ever do this
#include "ecs/ecs.cpp"
#include "platformer/logic.cpp"
#include "platformer/scene-json.cpp"
#include "platformer/input-frame.cpp"

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
	void vegicide_step (VgHandle);
	const char * vegicide_get_graphics_json (VgHandle);
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
	Logic logic;
	long frames;
	string graphics_json;
	
	VgContext () {
		emscripten_wget ("maps/demo.bin", "maps/demo.bin");
		
		level = Level (lookup_file ("maps/demo.bin"));
		logic = Logic (level);
		frames = 0;
	}
	
	void step () {
		InputFrame input_frame;
		logic.step (input_frame, frames);
		
		frames++;
	}
	
	const char * get_graphics_json () {
		GraphicsEcs graphics_ecs;
		graphics_ecs = animate_vegicide (logic.scene, logic.level, frames, 800.0 / 480.0);
		
		stringstream s;
		to_json (s, logic.scene);
		
		graphics_json = s.str ();
		
		return graphics_json.c_str ();
	}
};

VgHandle vegicide_init () {
	return new VgContext ();
}

void vegicide_step (VgHandle h) {
	VgContext & ctx = *(VgContext *)h;
	ctx.step ();
}

const char * vegicide_get_graphics_json (VgHandle h) {
	VgContext & ctx = *(VgContext *)h;
	return ctx.get_graphics_json ();
}

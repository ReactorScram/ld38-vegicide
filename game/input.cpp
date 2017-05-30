#include "input.h"

#include <iostream>

#include <SDL/SDL.h>

using namespace std;

void Input::clear_taps () {
	frame.clear_taps ();
}

void Input::process (const KeyEvent & ev) {
	bool debug = false; //true;
	
	auto b = ev.code;
	if (b < (int)InputButton::NUM_BUTTONS) {
		frame.buttons [b] = ev.down;
		frame.taps [b] = ev.down;
		if (debug) {
			cerr << b << ", " << ev.down << endl;
		}
	}
}

#include "input.h"

#include <iostream>

#include <SDL/SDL.h>

using namespace std;

InputButton map_key (int key) {
	switch (key) {
		case SDLK_LEFT:
		case SDLK_a:
		case SDLK_j:
			return InputButton::Left;
		case SDLK_RIGHT:
		case SDLK_d:
		case SDLK_l:
			return InputButton::Right;
		case SDLK_UP:
		case SDLK_w:
		case SDLK_i:
			return InputButton::Up;
		case SDLK_DOWN:
		case SDLK_s:
		case SDLK_k:
			return InputButton::Down;
		//case SDLK_SPACE:
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return InputButton::Pounce;
		case SDLK_r:
			return InputButton::Reset;
		case SDLK_F9:
			return InputButton::QuickSave;
		case SDLK_F5:
			return InputButton::QuickLoad;
		default:
			return InputButton::NUM_BUTTONS;
	}
}

void Input::clear_taps () {
	frame.clear_taps ();
}

KeyEvent Input::encode (const SDL_Event & ev) {
	KeyEvent rc;
	rc.code = 255;
	rc.down = false;
	
	switch (ev.type) {
		case SDL_KEYDOWN:
			rc.down = true;
			rc.code = (uint8_t)map_key (ev.key.keysym.sym);
			break;
		case SDL_KEYUP:
			rc.down = false;
			rc.code = (uint8_t)map_key (ev.key.keysym.sym);
			break;
	}
	
	return rc;
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

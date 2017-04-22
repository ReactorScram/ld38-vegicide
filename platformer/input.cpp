#include "input.h"

#include <iostream>

#include <SDL/SDL.h>

using namespace std;

InputFrame::InputFrame () {
	for (int i = 0; i < (int)InputButton::NUM_BUTTONS; i++) {
		buttons [i] = false;
	}
}

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
		default:
			return InputButton::NUM_BUTTONS;
	}
}

void Input::process (const SDL_Event & ev) {
	switch (ev.type) {
		case SDL_KEYDOWN: {
			InputButton b = map_key (ev.key.keysym.sym);
			if (b < InputButton::NUM_BUTTONS) {
				frame.buttons [(int)b] = true;
				cerr << (int)b << ", true" << endl;
			}
		}
			break;
		case SDL_KEYUP: {
			InputButton b = map_key (ev.key.keysym.sym);
			if (b < InputButton::NUM_BUTTONS) {
				frame.buttons [(int)b] = true;
				cerr << (int)b << ", false" << endl;
			}
		}
			break;
	}
}

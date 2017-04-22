#ifndef VEGICIDE_INPUT_H
#define VEGICIDE_INPUT_H

enum class InputButton {
	Right,
	Up,
	Left,
	Down,
	Pounce,
	NUM_BUTTONS,
};

struct InputFrame {
	bool buttons [(int)InputButton::NUM_BUTTONS];
	bool taps [(int)InputButton::NUM_BUTTONS];
	
	InputFrame ();
	void clear_taps ();
};

union SDL_Event;

struct Input {
	InputFrame frame;
	
	void clear_taps ();
	void process (const SDL_Event & ev);
};

#endif

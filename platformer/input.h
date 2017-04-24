#ifndef VEGICIDE_INPUT_H
#define VEGICIDE_INPUT_H

enum class InputButton {
	Right,
	Up,
	Left,
	Down,
	Pounce,
	Reset,
	QuickLoad,
	QuickSave,
	NUM_BUTTONS,
};

struct InputFrame {
	bool buttons [(int)InputButton::NUM_BUTTONS];
	bool taps [(int)InputButton::NUM_BUTTONS];
	
	bool was_tapped (InputButton) const;
	bool was_held (InputButton) const;
	bool was_any (InputButton) const;
	
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

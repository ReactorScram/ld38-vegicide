#include "input.h"

InputFrame::InputFrame () {
	for (int i = 0; i < (int)InputButton::NUM_BUTTONS; i++) {
		buttons [i] = false;
	}
	
	clear_taps ();
}

void InputFrame::clear_taps () {
	for (int i = 0; i < (int)InputButton::NUM_BUTTONS; i++) {
		taps [i] = false;
	}
}

bool InputFrame::was_tapped (InputButton b) const {
	return taps [(int)b];
}

bool InputFrame::was_held (InputButton b) const {
	return buttons [(int)b];
}

bool InputFrame::was_any (InputButton b) const {
	return was_tapped (b) || was_held (b);
}

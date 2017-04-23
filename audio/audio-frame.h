#ifndef IDK_AUDIO_FRAME_H
#define IDK_AUDIO_FRAME_H

#include <string>

enum class EMusic {
	NoMusic,
	Ambient,
	Boss,
};

// Match this to loadVorbises
enum class ESound {
	KingLaugh,
	KingPain,
	KingRoar,
	KingYou,
	NUM_SOUNDS,
};

struct AudioFrame {
	EMusic bgm;
	bool sounds [(int)ESound::NUM_SOUNDS];
	
	AudioFrame () {
		bgm = EMusic::Boss;
		for (int i = 0; i < (int)ESound::NUM_SOUNDS; i++) {
			sounds [i] = false;
		}
	}
};

#endif

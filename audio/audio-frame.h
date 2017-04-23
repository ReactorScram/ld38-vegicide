#ifndef IDK_AUDIO_FRAME_H
#define IDK_AUDIO_FRAME_H

#include <string>

enum class EMusic {
	NoMusic,
	Ambient,
	Boss,
};

struct AudioFrame {
	bool boostActivated;
	float throttle;
	bool dirt;
	
	EMusic bgm;
	
	std::string voiceTrigger;
	
	AudioFrame () {
		boostActivated = false;
		throttle = 0.0f;
		dirt = false;
		bgm = EMusic::Boss;
	}
};

#endif

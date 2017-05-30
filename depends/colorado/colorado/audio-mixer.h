#ifndef AUDIOMIXER_H
#define AUDIOMIXER_H

#include <fstream>
#include <vector>

#include <SDL/SDL_types.h>

#include "colorado/dllspec.h"

typedef struct Mix_Chunk Mix_Chunk;

namespace Colorado {
class COLORADO_DLLSPEC AudioChannel {
public:
	bool enabled;
	int volume;
	Mix_Chunk * chunk;
	bool repeat;
	
	// Unit is stereo samples, so 4 bytes = 1 offset
	int offset;
	float speedRatio;
	
	AudioChannel ();
};

class COLORADO_DLLSPEC AudioMixer {
public:
	// Call SDL_LockAudio and SDL_UnlockAudio when touching this
	std::vector <AudioChannel> channels;
	
	AudioMixer ();
	
	void startRecording (std::string);
	void stopRecording ();
	
	static void audioCallback (void *, Uint8 *, int);
	
protected:
	std::ofstream audioRecording;
};
}

#endif // AUDIOMIXER_H

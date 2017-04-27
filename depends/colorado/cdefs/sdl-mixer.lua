return [[
// SDL_mixer
typedef struct Mix_Chunk * Mix_Chunk_handle;
typedef struct Mix_Music * Mix_Music_handle;

int Mix_AllocateChannels(int numchans);
void Mix_FreeChunk(Mix_Chunk_handle chunk);
void Mix_FreeMusic(Mix_Music_handle music);
void Mix_HookMusicFinished (void (*music_finished)(void));
Mix_Music_handle Mix_LoadMUS (const char *);
Mix_Music_handle Mix_LoadMUS_RW(SDL_RW_handle rw);
Mix_Music_handle Mix_LoadMUSType_RW(SDL_RW_handle rw, int type, int freesrc);
Mix_Chunk_handle Mix_LoadWAV_RW(SDL_RW_handle src, int freesrc);
int Mix_OpenAudio(int frequency, uint16_t format, int channels,
							int chunksize);
int Mix_PausedMusic ();
void Mix_PauseMusic(void);
int Mix_PlayChannelTimed(int channel, Mix_Chunk_handle chunk, int loops, int ticks);
int Mix_PlayMusic(Mix_Music_handle music, int loops);
void Mix_Quit(void);
void Mix_ResumeMusic(void);
void Mix_SetPostMix (void (*mix_func)(void *, uint8_t *, int), void * arg);

int Mix_VolumeMusic (int);
]]

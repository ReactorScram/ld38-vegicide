return [[
// Colorado Audio
typedef void * Colorado_AudioRecordingHandle;

void Colorado_Lua_hookMusicFinished (void);
void Colorado_Lua_Audio_recordChunk (void *, uint8_t *, int);
Colorado_AudioRecordingHandle Colorado_Lua_Audio_startRecording (const char *);
void Colorado_Lua_Audio_stopRecording (Colorado_AudioRecordingHandle);
]]

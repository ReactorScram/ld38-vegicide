return [[
// Colorado Game
typedef struct {
	int width, height;
	bool fullscreen;
} Colorado_ScreenOptions;

void Colorado_Lua_Game_startPhysFs (const char *);
SDL_Surface * Colorado_Lua_Game_startSdl (const Colorado_ScreenOptions *);
void Colorado_Lua_Game_stopPhysFs ();
void Colorado_Lua_Game_stopSdl ();
]]
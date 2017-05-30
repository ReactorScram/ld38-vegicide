#ifndef LUASDL_H
#define LUASDL_H

#include "SDL2/SDL.h"

#include "colorado/dllspec.h"

struct lua_State;

namespace ColoradoLua {

COLORADO_DLLSPEC void pushSdlLib (lua_State *);
COLORADO_DLLSPEC void registerSDL_EventLib (lua_State *);

// Global SDL bindings
int colorado_SDL_Delay (lua_State *);
int colorado_SDL_GetKeyboardState (lua_State *);
int colorado_SDL_GetMouseState (lua_State *);
int colorado_SDL_GetTicks (lua_State *);
int colorado_SDL_GL_MakeCurrent (lua_State *);
int colorado_SDL_GL_SwapWindow (lua_State *);
int colorado_SDL_PollEvent (lua_State *);
int colorado_SDL_RenderClear (lua_State *);
int colorado_SDL_RenderPresent (lua_State *);
int colorado_SDL_SetRelativeMouseMode (lua_State *);
int colorado_SDL_SetWindowGrab (lua_State *);

// SDL event userdata
SDL_Event * checkSDL_Event (lua_State *, int idx = -1);
void colorado_push_SDL_Event (lua_State *, SDL_Event *);
int colorado_SDL_Event_new (lua_State *);
int colorado_SDL_Event_gc (lua_State *);
int colorado_SDL_Event_type (lua_State *);
int colorado_SDL_Event_key_scancode (lua_State *);
int colorado_SDL_Event_key_repeat (lua_State *);
int colorado_SDL_Event_mouseMotion_xrel (lua_State *);
int colorado_SDL_Event_mouseMotion_yrel (lua_State *);

}

#endif // LUASDL_H

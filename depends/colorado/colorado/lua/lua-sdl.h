#ifndef COLORADO_LUA_SDL_H
#define COLORADO_LUA_SDL_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

COLORADO_DLLSPEC int registerSdlLib (lua_State *);

}
}

#endif

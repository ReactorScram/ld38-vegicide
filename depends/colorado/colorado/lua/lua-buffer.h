#ifndef COLORADO_LUA_BUFFER_H
#define COLORADO_LUA_BUFFER_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

COLORADO_DLLSPEC int registerBufferLib (lua_State *);

}
}

#endif

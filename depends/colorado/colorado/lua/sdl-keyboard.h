#ifndef LUASDLKEYBOARD_H
#define LUASDLKEYBOARD_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

COLORADO_DLLSPEC void registerKeyboard (lua_State *);

}
}

#endif // LUASDLKEYBOARD_H

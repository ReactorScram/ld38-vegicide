#ifndef LUASDLJOYSTICK_H
#define LUASDLJOYSTICK_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

COLORADO_DLLSPEC void registerSdlInputLib (lua_State *);

}
}

#endif // LUASDLJOYSTICK_H

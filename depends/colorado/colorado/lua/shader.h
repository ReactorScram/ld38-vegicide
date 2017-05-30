#ifndef LUASHADER_H
#define LUASHADER_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {

class ProgramLinkResult;
class Shader;

namespace Lua {

COLORADO_DLLSPEC void registerShaderLib (lua_State *);

// Shader lightuserdata
Shader * toShader (lua_State *, int idx = -1);

void pushProgramLinkResult (lua_State *, const ProgramLinkResult &);

}
}

#endif // LUASHADER_H

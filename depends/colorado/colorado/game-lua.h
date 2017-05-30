#ifndef COLORADO_GAME_LUA_H
#define COLORADO_GAME_LUA_H

#include "lua.hpp"

#include "colorado/dllspec.h"

namespace Colorado {
class GameLuaPImpl;

class GameLua {
public:
	GameLua (int, char * []);
	~GameLua ();
	
	bool init ();
	void exec ();
	void deinit ();
	
	static int COLORADO_DLLSPEC main (int, char * []);
	
protected:
	GameLuaPImpl * pImpl;
};
}

extern "C" {

int COLORADO_DLLSPEC luaopen_libcolorado (lua_State *);

}

#endif // GAMELUA_H

#pragma once
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <iostream>

namespace E2
{
    inline bool CheckLua(lua_State* pState, int errorCode)
    {
        if (errorCode != LUA_OK)
        {
            const char* errormsg = lua_tostring(pState, -1);
            std::cout << errormsg << '\n';
            return false;
        }
        return true;
    }
}

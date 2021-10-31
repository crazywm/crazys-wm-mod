/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <CLog.h>
#include "cLuaState.h"
#include <boost/variant.hpp>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace scripting;

void cLuaState::load(const std::string &file) {
    if(luaL_loadfile(m_State, file.c_str())) {
        throw std::runtime_error("Could not load lua file: " + file + ": " + get_error());
    }

    if (pcall(0, 1)) {
        throw std::runtime_error("Error running lua file " + file + ": " + get_error());
    }
}

int cLuaState::pcall(int nargs, int nresults) {
    return lua_pcall(m_State, nargs, nresults, 0);
}

std::string cLuaState::get_error() {
    std::string errstr = lua_tostring(m_State, -1);
    lua_pop(m_State, 1);
    return std::move(errstr);
}

bool cLuaState::get_function(const std::string& name) {
    lua_getglobal(m_State, name.c_str());
    return lua_isfunction(m_State, -1);
}

bool cLuaState::has_function(const std::string& name) const {
    lua_getglobal(m_State, name.c_str());
    bool result = lua_isfunction(m_State, -1);
    lua_pop(m_State, 1);
    return result;
}

void cLuaState::settable(int index, const char *key, int value) {
    lua_pushstring(m_State, key);
    lua_pushinteger(m_State, value);
    lua_settable(m_State, index - 2);
}



sScriptValue scripting::get_value(lua_State* interpreter, int index) {
    if(lua_isnumber(interpreter, index)) {
        return sScriptValue((float)lua_tonumberx(interpreter, index, nullptr));
    } else if(lua_isstring(interpreter, index)) {
        return sScriptValue(std::string(lua_tostring(interpreter, index)));
    } else if(lua_isboolean(interpreter, index)) {
        return sScriptValue((bool)lua_toboolean(interpreter, index));
    } else {
        const char* top_as_str = lua_tostring(interpreter, index);
        g_LogFile.warning("lua", "Could not convert lua return value to C++ value: ", top_as_str);
        return boost::blank{};
    }
}

cLuaInterpreter::cLuaInterpreter() : cLuaState(luaL_newstate()) {
    if(!get_state())
        throw std::runtime_error("Could not create lua state");
    luaL_openlibs(get_state());

    // prepare the threads table
    lua_pushstring(get_state(), "_threads");
    lua_newtable(get_state());
    lua_settable(get_state(), LUA_REGISTRYINDEX);
}

cLuaInterpreter::~cLuaInterpreter() {
    auto state = get_state();
    if(state) lua_close(state);
}

sLuaThread* sLuaThread::create(lua_State* L) {
    lua_checkstack(L, 20);

    // create the new thread
    get_threads_table(L);

    // create a new thread and register it
    lua_State* thread_interpreter = lua_newthread(L);
    size_t nbytes = sizeof(sLuaThread);
    auto thread = (sLuaThread*)lua_newuserdata(L, nbytes);
    lua_settable(L, -3);
    // _threads[t] = sLuaThread
    g_LogFile.info("lua", "Creating thread ", thread_interpreter);

    // now construct the new thread
    new(thread) sLuaThread;
    thread->InterpreterState = thread_interpreter;

    return thread;
}

void sLuaThread::resume(int num_params) {
    // registers the current thread as the active one
    // first, get the global '_threads' table
    get_threads_table(InterpreterState);

    // second, set the active thread and clean the stack
    lua_pushstring(InterpreterState, "_active_thread");
    lua_pushthread(InterpreterState);
    lua_settable(InterpreterState, -3);
    lua_pop(InterpreterState, 1);

    // now we're ready to call
    int result = lua_resume(InterpreterState, nullptr, num_params);
    if(result != 0 && result != LUA_YIELD) {
        std::string errstr = lua_tostring(InterpreterState, -1);
        lua_pop(InterpreterState, 1);
        g_LogFile.error("scripting", std::move(errstr));
    } else if(result == 0) {
        // thread is finished -- signal the continuation
        int top = lua_gettop(InterpreterState);
        if(top > 0 && DoneHandler) {
            DoneHandler(get_value(InterpreterState, top));
        } else if (DoneHandler) {
            DoneHandler(boost::blank{});
        }

        // clean up the thread
        destroy();
    }
}

void sLuaThread::destroy() {
    get_threads_table(InterpreterState);

    // remove entry from threads table
    lua_pushthread(InterpreterState);
    lua_pushnil(InterpreterState);
    lua_rawset(InterpreterState, -3);

    g_LogFile.info("lua", "Deleting thread #", InterpreterState);

    // finally, clean up myself
    this->~sLuaThread();
}

void sLuaThread::get_threads_table(lua_State* L) {
    lua_pushstring(L, "_threads");
    int result_type = lua_gettable(L, LUA_REGISTRYINDEX);
    if(result_type != LUA_TTABLE) {
        g_LogFile.error("lua", "Could not get threads table! Got ", lua_typename(L, result_type));
        throw std::runtime_error("Could not get lua threads table");
    }
}

sLuaThread* sLuaThread::get_active_thread(lua_State* L) {
    get_threads_table(L);
    lua_pushthread(L);
    lua_rawget(L, -2);
    void* result = lua_touserdata(L, -1);
    assert(result);
    lua_pop(L, 2);
    return static_cast<sLuaThread*>(result);
}


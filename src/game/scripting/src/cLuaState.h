/*
 * Copyright 2019-2022, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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

#ifndef CRAZYS_WM_MOD_CLUASTATE_H
#define CRAZYS_WM_MOD_CLUASTATE_H

#include <string>
#include <functional>
#include "scripting.h"

class lua_State;

namespace scripting {
    struct sLuaThread {
        lua_State* InterpreterState;
        std::function<void(sScriptValue)> DoneHandler;

        static sLuaThread* create(lua_State* L);
        static sLuaThread* get_active_thread(lua_State* L);
        static void get_threads_table(lua_State* L);

        void resume(int num_params);
        void destroy();
    };

    class cLuaState {
    public:
        explicit cLuaState(lua_State *state, int thread_id=0) :
            m_State(state), m_ThreadID(thread_id) {

        };

        void load(const std::string &file);

        // implement commonly used standard lua functions
        int pcall(int nargs, int nresults);
        void settable(int index, const char* key, int value);
        void settable(int index, const char* key, const std::string& value);
        void settable(int index, const std::string& key, int value) { settable(index, key.c_str(), value); }
        void settable(int index, const std::string& key, const std::string& value) { settable(index, key.c_str(), value); }

        // common operations
        bool get_function(const std::string& name);
        bool has_function(const std::string& name) const;

        lua_State* get_state() { return m_State; }
        std::string get_error();
    private:
        lua_State* m_State = nullptr;
        /// If this state represents a specific thread, this variable saves the thread id.
        int m_ThreadID = 0;
    };

    // lua helper functions
    sScriptValue get_value(lua_State* interpreter, int index);

    // A cLuaState class with owns the underlying lua_State,
    // i.e a new lua state is created in the constructor and
    // closed in the destructor.
    struct cLuaInterpreter : public cLuaState {
        cLuaInterpreter();
        ~cLuaInterpreter();
    };
}

#endif //CRAZYS_WM_MOD_CLUASTATE_H

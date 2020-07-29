/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
class lua_State;

namespace scripting {
    class cLuaState {
    public:
        explicit cLuaState(lua_State *state, int thread_id=0) :
            m_State(state), m_ThreadID(thread_id) {

        };

        void load(const std::string &file);

        // implement commonly used standard lua functions
        int pcall(int nargs, int nresults);
        void settable(int index, const char* key, int value);
        void settable(int index, const std::string& key, int value) { settable(index, key.c_str(), value); }

        cLuaState newthread() const;

        // common operations
        bool get_function(const std::string &name);

        lua_State *get_state() { return m_State; }

        std::string get_error();

        void CleanThread();
    private:
        lua_State* m_State = nullptr;
        /// If this state represents a specific thread, this variable saves the thread id.
        int m_ThreadID = 0;
    };

    // A cLuaState class with owns the underlying lua_State,
    // i.e a new lua state is created in the constructor and
    // closed in the destructor.
    struct cLuaInterpreter : public cLuaState {
        cLuaInterpreter();
        ~cLuaInterpreter();
    };
}

#endif //CRAZYS_WM_MOD_CLUASTATE_H

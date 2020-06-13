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

#ifndef CRAZYS_WM_MOD_CLUASCRIPT_H
#define CRAZYS_WM_MOD_CLUASCRIPT_H

#include <string>
#include "cLuaState.h"
#include <initializer_list>
class sGirl;

namespace scripting {
    class sLuaParameter;

    class cLuaScript {
    public:
        explicit cLuaScript();
        void LoadSource(const std::string& file);

        sScriptValue RunEvent(const std::string &event_name, std::initializer_list<sLuaParameter> params);
    private:
        void PushParameter(sLuaParameter param);
        cLuaInterpreter m_State;

    public:
        // lua functions
        static int ChoiceBox(lua_State* state);
        static int Dialog(lua_State* state);
        static int GameOver(lua_State* state);
        static int GivePlayerRandomSpecialItem(lua_State* state);
        static int AddCustomerToDungeon(lua_State* state);
        static int AddFamilyToDungeon(lua_State* state);
        static int UpdateImage(lua_State* state);

        // random boolean with the given chance of being true
        static int Percent(lua_State* state);
        static int Range(lua_State* state);
    };
}


#endif //CRAZYS_WM_MOD_CLUASCRIPT_H

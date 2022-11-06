/*
 * Copyright 2022 The Pink Petal Development Team.
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

#ifndef WM_SLUAEVENTRESULT_H
#define WM_SLUAEVENTRESULT_H

#include "scripting.h"
#include "lua.hpp"

namespace scripting {
    void create_event_result(lua_State* L, sLuaEventResult* result);
    void init_event_result(lua_State* L);
}

#endif //WM_SLUAEVENTRESULT_H

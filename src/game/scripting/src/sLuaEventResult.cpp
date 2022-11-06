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

#include "sLuaEventResult.h"
#include "images/sImageSpec.h"
#include "cLuaState.h"
#include <iostream>

using namespace scripting;

namespace scripting {
    void create_event_result(lua_State* L, sLuaEventResult* result) {
        size_t nbytes = sizeof(sLuaEventResult**);
        auto a = (sLuaEventResult**)lua_newuserdata(L, nbytes);
        *a = result;

        luaL_getmetatable(L, "wm.EventResult");
        lua_setmetatable(L, -2);
    }

    sLuaEventResult& check_event_result(lua_State* L, int index) {
        void *ud = luaL_checkudata(L, index, "wm.EventResult");
        luaL_argcheck(L, ud != nullptr, index, "`wm.EventResult' expected");
        sLuaEventResult* gp = *(sLuaEventResult**)ud;
        luaL_argcheck(L, gp != nullptr, index, "pointer is null");
        return *gp;
    }

    int event_result_add_text(lua_State* L) {
        auto& res = check_event_result(L, 1);
        const char* text = luaL_checkstring(L, 2);
        if(!res.Text.empty() && !std::isspace(res.Text.back()) ) {
            res.Text.push_back(' ');
        }
        res.Text += text;
        return 0;
    }

    int event_result_set_text(lua_State* L) {
        auto& res = check_event_result(L, 1);
        const char* text = luaL_checkstring(L, 2);
        res.Text = text;
        return 0;
    }

    int event_result_set_image(lua_State* state) {
        auto& res = check_event_result(state, 1);
        long image_type = luaL_checkinteger(state, 2);

        sImagePreset preset = [&]()-> sImagePreset {
            if(image_type >= (int)EImageBaseType::NUM_TYPES) {
                return (EImagePresets)(image_type  - (int)EImageBaseType::NUM_TYPES);
            } else {
                return (EImageBaseType)(image_type);
            }
        }();

        res.Image->BasicImage = preset.base_image();
        res.Image->IsTied = preset.tied_up();
        res.Image->Participants = preset.participants();

        if(lua_gettop(state) == 3) {
            // TODO turn this into a utility function
            lua_getfield(state, 3, "participants");
            if (!lua_isnil(state, 4)) {
                long participant_type = luaL_checkinteger(state, 4);
                res.Image->Participants = (ESexParticipants) participant_type;
            }
            lua_pop(state, 1);

            lua_getfield(state, 3, "tied");
            if (!lua_isnil(state, 4)) {
                long is_tied = lua_toboolean(state, 4);
                res.Image->IsTied = is_tied ? ETriValue::Yes : ETriValue::No;
            }
            lua_pop(state, 1);
        }

        return 0;
    }

    void init_event_result(lua_State* L) {
        luaL_newmetatable(L, "wm.EventResult");
        cLuaState state{L};
        // metatable.__index = metatable
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);

        // fill in the metatable
        luaL_Reg methods[] = {
                {"add_text", event_result_add_text},
                {"set_text", event_result_set_text},
                {"set_image", event_result_set_image},
                {nullptr, nullptr}
        };
        luaL_setfuncs(L, methods, 0);

        // clean up the stack -- remove the new meta-table
        lua_pop(L, 1);
    }
}

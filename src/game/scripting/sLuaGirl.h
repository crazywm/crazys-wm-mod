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

#ifndef CRAZYS_WM_MOD_SLUAGIRL_H
#define CRAZYS_WM_MOD_SLUAGIRL_H

class sGirl;
class sCustomer;
struct lua_State;

template<class T>
struct sCharacter {
    static int getset_stat(lua_State* L);
    static int getset_skill(lua_State* L);

    static int up_getset_stat(lua_State* L);
    static int up_getset_skill(lua_State* L);
};

struct sLuaGirl : public sCharacter<sLuaGirl>{
    static void init(lua_State* L);
    static void create(lua_State* state, sGirl* girl);
    static sGirl& check_type(lua_State* L, int index);

    static int add_trait(lua_State* L);
    static int has_trait(lua_State* L);
    static int remove_trait(lua_State* L);

    static int check_virginity(lua_State* L);
    static int lose_virginity(lua_State* L);

    static int obey_check(lua_State* L);

    static int calc_player_pregnancy(lua_State* L);
    static int clear_pregnancy(lua_State* L);
    static int is_pregnant(lua_State* L);
    static int start_pregnancy(lua_State* L);
    static int weeks_pregnant(lua_State* L);
    static int pregnancy_term(lua_State* L);

    static int set_status(lua_State* L);
    static int has_status(lua_State* L);

    static int get_name(lua_State* L);
    static int is_slave(lua_State* L);

    static int has_item(lua_State* L);
    static int give_money(lua_State* L);

    static int add_message(lua_State* L);

    // sex


    // global functions
    static int acquire_girl(lua_State* L);
    static int to_dungeon(lua_State* L);
    static int torture(lua_State* L);
    /// TODO memory management
    static int create_random_girl(lua_State* L);
};

struct sLuaCustomer : public sCharacter<sLuaCustomer>{
    static void init(lua_State* L);
    static void create(lua_State* L, sCustomer* girl);
    static sCustomer& check_type(lua_State* L, int index);
};

#endif //CRAZYS_WM_MOD_SLUAGIRL_H

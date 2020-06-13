extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <algorithm>
#include "character/pregnancy.h"
#include "character/cCustomers.h"
#include "character/cPlayer.h"

#include "character/sGirl.hpp"
#include "Game.hpp"
#include "buildings/cDungeon.h"
#include "IBuilding.hpp"
#include "sLuaGirl.h"
#include "cLuaState.h"
#include "utils/string.hpp"
#include "cGirlTorture.h"
#include "character/predicates.h"
#include "buildings/cBrothel.h"

using namespace scripting;

void sLuaGirl::init(lua_State* L) {
    luaL_newmetatable(L, "wm.Girl");

    cLuaState state{L};
    // metatable.__index = metatable
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    // fill in the metatable
    luaL_Reg methods[] = {
            {"stat", sLuaGirl::getset_stat},
            {"skill", sLuaGirl::getset_skill},
            {"add_trait", sLuaGirl::add_trait},
            {"has_trait", sLuaGirl::has_trait},
            {"remove_trait", sLuaGirl::remove_trait},
            {"has_item", sLuaGirl::has_item},
            {"check_virginity", sLuaGirl::check_virginity},
            {"lose_virginity", sLuaGirl::lose_virginity},
            {"name", sLuaGirl::get_name},
            {"is_slave", sLuaGirl::is_slave},
            {"calc_player_pregnancy", sLuaGirl::calc_player_pregnancy},
            {"clear_pregnancy", sLuaGirl::clear_pregnancy},
            {"is_pregnant", sLuaGirl::is_pregnant},
            {"start_pregnancy", sLuaGirl::start_pregnancy},
            {"weeks_pregnant", sLuaGirl::weeks_pregnant},
            {"pregnancy_term", sLuaGirl::pregnancy_term},
            {"set_status", sLuaGirl::set_status},
            {"has_status", sLuaGirl::has_status},
            {"obey_check", sLuaGirl::obey_check},
            {"add_message", sLuaGirl::add_message},
            {"give_money", sLuaGirl::give_money},
            {"torture", sLuaGirl::torture},
            {nullptr, nullptr}
    };
    luaL_setfuncs(L, methods, 0);

    // register shorthands for skill functions
    for(int i = 0; i < NUM_STATS; ++i) {
        lua_pushstring(L, tolower(get_stat_name((STATS)i)).c_str());
        lua_pushinteger(L, i);
        lua_pushcclosure(L, up_getset_stat, 1);
        lua_settable(L, -3);
    }

    for(int i = 0; i < NUM_SKILLS; ++i) {
        lua_pushstring(L, tolower(get_skill_name((SKILLS)i)).c_str());
        lua_pushinteger(L, i);
        lua_pushcclosure(L, up_getset_skill, 1);
        lua_settable(L, -3);
    }
}

void sLuaGirl::create(lua_State* L, sGirl* girl) {
    size_t nbytes = sizeof(sGirl**);
    auto a = (sGirl**)lua_newuserdata(L, nbytes);
    *a = girl;

    luaL_getmetatable(L, "wm.Girl");
    lua_setmetatable(L, -2);
}

sGirl& sLuaGirl::check_type(lua_State* L, int index) {
    void *ud = luaL_checkudata(L, index, "wm.Girl");
    luaL_argcheck(L, ud != nullptr, index, "`Girl' expected");
    sGirl* gp = *(sGirl**)ud;
    luaL_argcheck(L, gp != nullptr, index, "girl pointer is null");
    return *gp;
}

template<class T>
int sCharacter<T>::getset_stat(lua_State* L) {
    auto& girl = T::check_type(L, 1);
    int stat = luaL_checkinteger(L, 2);
    if(lua_gettop(L) == 3) {
        int value = luaL_checkinteger(L, 3);
        girl.upd_base_stat(stat, value);
        return 0;
    } else {
        int value = girl.get_stat(stat);
        lua_pushinteger(L, value);
        return 1;
    }
}

template<class T>
int sCharacter<T>::getset_skill(lua_State* L) {
    auto& girl = T::check_type(L, 1);
    int stat = luaL_checkinteger(L, 2);
    if(lua_gettop(L) == 3) {
        int value = luaL_checkinteger(L, 3);
        girl.upd_skill(stat, value);
        return 0;
    } else {
        int value = girl.get_skill(stat);
        lua_pushinteger(L, value);
        return 1;
    }
}

template<class T>
int sCharacter<T>::up_getset_skill(lua_State *L) {
    auto& girl = T::check_type(L, 1);
    int stat = lua_tointeger(L, lua_upvalueindex(1));
    if(lua_gettop(L) == 2) {
        int value = luaL_checkinteger(L, 2);
        girl.upd_skill(stat, value);
        return 0;
    } else {
        int value = girl.get_skill(stat);
        lua_pushinteger(L, value);
        return 1;
    }
}

template<class T>
int sCharacter<T>::up_getset_stat(lua_State *L) {
    auto& girl = T::check_type(L, 1);
    int stat = lua_tointeger(L, lua_upvalueindex(1));
    if(lua_gettop(L) == 2) {
        int value = luaL_checkinteger(L, 2);
        girl.upd_base_stat(stat, value);
        return 0;
    } else {
        int value = girl.get_stat(stat);
        lua_pushinteger(L, value);
        return 1;
    }
}

int sLuaGirl::add_trait(lua_State *L) {
    auto& girl = check_type(L, 1);
    const char* trait = luaL_checkstring(L, 2);
    int temp_time = 0;
    if(lua_gettop(L) == 3) {
        temp_time = luaL_checkinteger(L, 3);
    }
    if(temp_time > 0) {
        girl.add_temporary_trait(trait, temp_time);
    } else {
        girl.gain_trait(trait);
    }
    return 0;
}

int sLuaGirl::has_trait(lua_State *L) {
    auto& girl = check_type(L, 1);
    bool has = false;
    // take an arbitrary number of trait names, return whether one of them is present
    for(int i = 2; i <= lua_gettop(L); ++i) {
        const char* trait = luaL_checkstring(L, 2);
        has |= girl.has_active_trait(trait);
    }
    lua_pushboolean(L, has);
    return 1;
}

int sLuaGirl::remove_trait(lua_State *L) {
    auto& girl = check_type(L, 1);
    const char* trait = luaL_checkstring(L, 2);
    girl.lose_trait(trait);
    return 0;
}


int sLuaGirl::get_name(lua_State *L) {
    auto& girl = check_type(L, 1);
    lua_pushstring(L, girl.FullName().c_str());
    return 1;
}

int sLuaGirl::calc_player_pregnancy(lua_State *L) {
    auto& girl = check_type(L, 1);

    bool preg = false;
    if(lua_gettop(L) == 2) {
        double factor = luaL_checknumber(L, 2);
        preg = girl.calc_pregnancy(&g_Game->player(), factor);
    } else {
        preg = girl.calc_pregnancy(&g_Game->player());
    }
    lua_pushboolean(L, !preg);
    return 1;
}

int sLuaGirl::acquire_girl(lua_State* L) {
    auto& girl = check_type(L, 1);
    /* MYR: For some reason I can't figure out, a number of girl's house percentages
            are at zero or set to zero when they are sent to the brothel. I'm not sure
            how to fix it, so I'm explicitly setting the percentage to 60 here */
    girl.house(60);

    string text = girl.FullName();
/*
*    OK: how rebellious is this floozy?
*/
    if(cGirls::GetRebelValue(&girl, false) >= 35) {
        text += " has been sent to your dungeon, as she is rebellious and poorly trained.";
        g_Game->push_message(text, 0);
        g_Game->dungeon().AddGirl(&girl, DUNGEON_NEWGIRL);
        return 0;
    }
/*
*    She qualifies for brothel duty - is there room?
*    let's get some numbers
*/
    // TODO figure out which building is used here
    IBuilding& building = g_Game->buildings().get_building(0);
    int total_rooms = building.m_NumRooms;
    int rooms_used  = building.num_girls();
    int diff = total_rooms - rooms_used;
/*
*    now then...
*/
    if(diff <= 0) {
        text += (" has been sent to your dungeon, since current brothel is full.");
        g_Game->push_message(text, 0);
        g_Game->dungeon().AddGirl(&girl, DUNGEON_NEWGIRL);
        return 0;
    }
/*
*    otherwise, it's very simple
*/
    text += (" has been sent to your current brothel.");
    building.add_girl(&girl);
    g_Game->push_message(text, 0);
    return 0;
}

int sLuaGirl::create_random_girl(lua_State *L) {
    int age = luaL_checkinteger(L, 1);
    bool slave = lua_toboolean(L, 2);
    bool non_human = lua_toboolean(L, 3);
    bool kidnapped = lua_toboolean(L, 4);
    bool arena = lua_toboolean(L, 5);
    bool daughter = lua_toboolean(L, 6);
    sGirl* newgirl = g_Game->CreateRandomGirl(age, slave, false, non_human, kidnapped, arena, daughter).release();
    create(L, newgirl);
    return 1;
}

int sLuaGirl::to_dungeon(lua_State *L) {
    auto& girl = check_type(L, 1);
    int reason = luaL_checkinteger(L, 2);
    g_Game->dungeon().AddGirl(&girl, reason);
    return 0;
}

int sLuaGirl::is_slave(lua_State *L) {
    auto& girl = check_type(L, 1);
    bool slave = girl.is_slave();
    lua_pushboolean(L, slave);
    return 1;
}

int sLuaGirl::has_item(lua_State *L) {
    auto& girl = check_type(L, 1);
    const char* item = luaL_checkstring(L, 2);
    bool has = girl.has_item(item);
    lua_pushboolean(L, has);
    return 1;
}

int sLuaGirl::check_virginity(lua_State *L) {
    auto& girl = check_type(L, 1);
    lua_pushboolean(L, is_virgin(girl));
    return 1;
}

int sLuaGirl::lose_virginity(lua_State *L) {
    auto& girl = check_type(L, 1);
    bool result = girl.lose_trait("Virgin");
    lua_pushboolean(L, result);
    return 1;
}

int sLuaGirl::add_message(lua_State *L) {
    auto& girl = check_type(L, 1);
    std::string message = luaL_checkstring(L, 2);
    int imgtype = luaL_checkinteger(L, 3);
    int evtype = luaL_checkinteger(L, 4);
    girl.AddMessage(message, imgtype, evtype);
    return 0;
}

int sLuaGirl::clear_pregnancy(lua_State *L) {
    auto& girl = check_type(L, 1);
    girl.clear_pregnancy();
    return 0;
}

int sLuaGirl::is_pregnant(lua_State *L) {
    auto& girl = check_type(L, 1);
    bool result = girl.is_pregnant();
    lua_pushboolean(L, result);
    return 1;
}

int sLuaGirl::start_pregnancy(lua_State *L) {
    auto& girl = check_type(L, 1);
    int by_whom = luaL_checkinteger(L, 2);

    if (by_whom == 0)                    // STATUS_PREGNANT_BY_PLAYER
    {
        create_pregnancy(girl, 1, STATUS_PREGNANT_BY_PLAYER, g_Game->player());
    }
    else
    {
        sCustomer Cust;
        int status = (by_whom == 1 ? STATUS_PREGNANT : STATUS_INSEMINATED);
        create_pregnancy(girl, 1, status, Cust);
    }
    return 0;
}

int sLuaGirl::set_status(lua_State *L) {
    auto& girl = check_type(L, 1);
    unsigned status = luaL_checkinteger(L, 2);
    int setto = luaL_checkinteger(L, 3);
    if (status == STATUS_PREGNANT || status == STATUS_PREGNANT_BY_PLAYER || status == STATUS_INSEMINATED)    // if creating pregnancy, remove old pregnancies
    {
        luaL_error(L, "use `start_pregnancy` and `clear_pregnancy` for pregnancy related status");
    }
    else
    {
        // Set value
        if (setto)    girl.set_status((STATUS)status);
        else         girl.remove_status((STATUS)status);
    }
    return 0;
}

int sLuaGirl::has_status(lua_State* L) {
    auto& girl = check_type(L, 1);
    unsigned status = luaL_checkinteger(L, 2);
    bool has = girl.has_status((STATUS)status);
    lua_pushboolean(L, has);
    return 1;
}

int sLuaGirl::obey_check(lua_State * L)
{
    auto& girl = check_type(L, 1);
    auto action = ACTION_GENERAL;
    if(lua_gettop(L) == 2) {
        action = (Action_Types)luaL_checkinteger(L, 2);
    }

    bool disobey = girl.disobey_check(action);
    lua_pushboolean(L, !disobey);
    return 1;
}

int sLuaGirl::torture(lua_State* L) {
    auto& girl = check_type(L, 1);
    cGirlTorture gt(&girl);
    return 0;
}

int sLuaGirl::give_money(lua_State * L)
{
    auto& girl = check_type(L, 1);
    int gold = luaL_checkinteger(L, 2);
    if(gold < 0)
        luaL_error(L, "Use `take_money` function if you want to take money from a girl.");

    girl.m_Money += gold;
    g_Game->gold().misc_credit(-gold);
    return 0;
}

int sLuaGirl::weeks_pregnant(lua_State* L)
{
    auto& girl = check_type(L, 1);
    lua_pushinteger(L, girl.m_WeeksPreg);
    return 1;
}

int sLuaGirl::pregnancy_term(lua_State* L)
{
    auto& girl = check_type(L, 1);
    lua_pushinteger(L, girl.get_preg_duration());
    return 1;
}



void sLuaCustomer::init(lua_State* L) {
    luaL_newmetatable(L, "wm.Customer");

    cLuaState state{L};
    // metatable.__index = metatable
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    // fill in the metatable
    luaL_Reg methods[] = {
            {"stat", getset_stat},
            {"skill", getset_skill},
            {nullptr, nullptr}
    };
    luaL_setfuncs(L, methods, 0);

    // register shorthands for skill functions
    for(int i = 0; i < NUM_STATS; ++i) {
        lua_pushstring(L, tolower(get_stat_name((STATS)i)).c_str());
        lua_pushinteger(L, i);
        lua_pushcclosure(L, up_getset_stat, 1);
        lua_settable(L, -3);
    }

    for(int i = 0; i < NUM_SKILLS; ++i) {
        lua_pushstring(L, tolower(get_skill_name((SKILLS)i)).c_str());
        lua_pushinteger(L, i);
        lua_pushcclosure(L, up_getset_skill, 1);
        lua_settable(L, -3);
    }
}

void sLuaCustomer::create(lua_State* L, sCustomer* cust) {
    size_t nbytes = sizeof(sCustomer**);
    auto a = (sCustomer**)lua_newuserdata(L, nbytes);
    *a = cust;

    luaL_getmetatable(L, "wm.Customer");
    lua_setmetatable(L, -2);
}

sCustomer& sLuaCustomer::check_type(lua_State* L, int index) {
    void *ud = luaL_checkudata(L, index, "wm.Customer");
    luaL_argcheck(L, ud != nullptr, index, "`wmCustomer' expected");
    sCustomer* gp = *(sCustomer**)ud;
    luaL_argcheck(L, gp != nullptr, index, "girl pointer is null");
    return *gp;
}

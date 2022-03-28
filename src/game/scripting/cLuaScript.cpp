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
#include <vector>

#include "interface/fwd.hpp"
#include "interface/cWindowManager.h"
#include "screens/cGameWindow.h"

#include "CLog.h"
#include "cNameList.h"
#include "IGame.h"
#include "cGold.h"
#include "cInventory.h"
#include "sStorage.h"
#include "Inventory.h"
#include "utils/string.hpp"

#include "character/cPlayer.h"
#include "character/sGirl.h"
#include "buildings/cDungeon.h"

#include "cLuaScript.h"
#include "cLuaState.h"
#include "sLuaGirl.h"
#include "sLuaParameter.hpp"


extern cRng g_Dice;
extern cNameList g_SurnameList;

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace scripting;

template <class F>
auto cify_int_arg(F&& f) {
    static F fn = std::forward<F>(f);
    return [] (lua_State* state) -> int {
        int arg = luaL_checkinteger(state, -1);
        fn(arg);
        return 0;
    };
}

template <class F>
auto cify_int_arg_ret(F&& f) {
    static F fn = std::forward<F>(f);
    return [] (lua_State* state) -> int {
        int arg = luaL_checkinteger(state, -1);
        lua_pushinteger(state, fn(arg));
        return 1;
    };
}

template <class F>
auto cify_no_arg(F&& f) {
    static F fn = std::forward<F>(f);
    return [] (lua_State* state) -> int {
        lua_pushinteger(state, fn());
        return 1;
    };
}

static const luaL_Reg funx [] = {
        { "ChoiceBox",                   cLuaScript::ChoiceBox },
        { "Dialog",                      cLuaScript::Dialog },
        { "UpdateImage",                 cLuaScript::UpdateImage },
        { "SetPlayerSuspicion",          cify_int_arg([](int amount) { g_Game->player().suspicion(amount); })},
        { "GetPlayerSuspicion",          cify_no_arg([](){ return g_Game->player().suspicion(); })},
        { "SetPlayerDisposition",        cify_int_arg([](int amount) { g_Game->player().disposition(amount); })},
        { "GetPlayerDisposition",        cify_no_arg([](){ return g_Game->player().disposition(); })},
        /// TODO make separate fns for adding and for requesting beasts
        { "AddBeasts",                   cify_int_arg([](int amount) { g_Game->storage().add_to_beasts(amount); })},
        { "GetBeasts",                   cify_no_arg([](){ return g_Game->storage().beasts(); })},
        { "AddPlayerGold",               cify_int_arg([](int amount) { g_Game->gold().misc_credit(amount); })},
        { "TakePlayerGold",              cify_int_arg_ret([](int amount) { return g_Game->gold().misc_debit(amount); })},
        { "GameOver",                    cLuaScript::GameOver},
        { "GivePlayerRandomSpecialItem", cLuaScript::GivePlayerRandomSpecialItem},
        { "AddCustomerToDungeon",        cLuaScript::AddCustomerToDungeon},
        { "AddFamilyToDungeon",          cLuaScript::AddFamilyToDungeon},
        // rng
        { "Range",                       cLuaScript::Range},
        { "Percent",                     cLuaScript::Percent},
        // girl
        {"AcquireGirl",                  sLuaGirl::acquire_girl},
        {"CreateRandomGirl",             sLuaGirl::create_random_girl},
        {"ToDungeon",                    sLuaGirl::to_dungeon},
        { nullptr,                       nullptr }
};

template<std::size_t N>
void register_table(const char* table, cLuaInterpreter& state, std::array<const char*, N> names) {
    lua_pushstring(state.get_state(), table);
    lua_newtable(state.get_state());
    for(int i = 0; i < N; ++i) {
        state.settable(-1, toupper(names[i]), i);
    }
    lua_settable(state.get_state(), -3);
}

cLuaScript::cLuaScript()
{
    auto L = m_State.get_state();
    luaL_newlib(m_State.get_state(), funx);
    lua_pushstring(L, "STATS");
    lua_newtable(L);
    for(auto stat: StatsRange) {
        m_State.settable(-1, toupper(get_stat_name(stat)), stat);
    }
    lua_settable(L, -3);


    lua_pushstring(L, "SKILLS");
    lua_newtable(L);
    for(auto skill: SkillsRange) {
        m_State.settable(-1, toupper(get_skill_name(skill)), skill);
    }
    lua_settable(L, -3);

    register_table("FETISH", m_State, get_fetish_names());
    register_table("STATUS", m_State, get_status_names());
    register_table("ACTIONS", m_State, get_action_names());
    register_table("IMG", m_State, get_imgtype_names());
    register_table("SPAWN", m_State, get_spawn_names());
    lua_setglobal(m_State.get_state(), "wm");
    sLuaGirl::init(m_State.get_state());
    sLuaCustomer::init(m_State.get_state());
}

sLuaThread* cLuaScript::RunAsync(const std::string& event_name, std::initializer_list<sLuaParameter> params) {
    g_LogFile.info("lua", "Running function '", event_name, "'");
    // create new thread
    auto thread = sLuaThread::create(m_State.get_state());
    auto ts = cLuaState(thread->InterpreterState);
    // find function
    if(!ts.get_function(event_name)) {
        g_LogFile.error("scripting", "Could not find lua function '", event_name, "'");
        throw std::runtime_error("Could not find lua function");
    }

    for(auto& arg : params) {
        arg.push(ts);
    }

    // run thread
    thread->resume(params.size());
    return thread;
}

sScriptValue cLuaScript::RunSynchronous(const std::string& event_name, std::initializer_list<sLuaParameter> params) {
    g_LogFile.info("lua", "Running sync function '", event_name, "'");
    // find function
    if(!m_State.get_function(event_name)) {
        g_LogFile.error("scripting", "Could not find lua function '", event_name, "'");
        throw std::runtime_error("Could not find lua function");
    }

    for(auto& arg : params) {
        arg.push(m_State);
    }

    // run thread
    lua_State* s = m_State.get_state();
    if(lua_pcall(s, params.size(), 1, 0)) {
        std::string error = m_State.get_error();
        g_LogFile.error("scripting", m_State.get_error());
        throw std::runtime_error(error);
    } else {
        int top = lua_gettop(s);
        if(top > 0) {
            return get_value(s, top);
        }
    }
    return boost::blank{};
}


int cLuaScript::ChoiceBox(lua_State* state)
{
    int nargs = lua_gettop(state);
    std::string question;
    std::vector<std::string> options;
    for(int i = 1; i <= nargs; ++i) {
        const char* option = luaL_checkstring(state, i);

        if(i == 1) {
            question = option;
        } else {
            options.emplace_back(option);
        }
    }

    auto callback = [state](int choice) {
        sLuaThread* thread = sLuaThread::get_active_thread(state);
        lua_pushinteger(state, choice);
        thread->resume(1);
    };

    window_manager().InputChoice(question, std::move(options), callback);
    return 0;
}

int cLuaScript::Dialog(lua_State* state)
{
    std::string text = luaL_checkstring(state, -1);
    window_manager().PushMessage(text, 0, [state](){
        sLuaThread* thread = sLuaThread::get_active_thread(state);
        thread->resume(0);
    });
    return 0;
}

int cLuaScript::GameOver(lua_State* state) {
    // TODO how do we handle this message?
    g_Game->push_message("GAME OVER", COLOR_RED);
    window_manager().PopAll();
    return 0;
}

int cLuaScript::GivePlayerRandomSpecialItem(lua_State* state) {
    auto filter
       = [](sInventoryItem const& item) {
        return item.m_Rarity >= Item_Rarity::SHOP05;
         };

    const sInventoryItem* item = g_Game->inventory_manager().GetRandomItem(filter);
    if(!item)
    {
       g_Game->push_message(" There are no suitable items to be had\n", COLOR_RED);
       return 0;
    }

    if(!g_Game->player().inventory().add_item(item)) {
        /// TODO how to handle this message?
        g_Game->push_message(" Your inventory is full\n", COLOR_RED);
    }
    return 0;
}

int cLuaScript::Percent(lua_State* state) {
    double prob = luaL_checknumber(state, 1);
    bool result = g_Dice.percent(prob);
    lua_pushboolean(state, result);
    return 1;
}

int cLuaScript::Range(lua_State *state) {
    long lower = luaL_checkinteger(state, 1);
    long upper = luaL_checkinteger(state, 2);
    int result = g_Dice.in_range(lower, upper);
    lua_pushinteger(state, result);
    return 1;
}

int cLuaScript::UpdateImage(lua_State* state) {
    long image_type = luaL_checkinteger(state, 1);
    auto top_window = window_manager().GetWindow(false);
    if(auto gw = dynamic_cast<cGameWindow*>(top_window)) {
        gw->UpdateImage(image_type);
    } else {
        g_LogFile.warning("scripting", "Script cannot set image on current screen");
    }
    return 0;
}


int cLuaScript::AddCustomerToDungeon(lua_State* state) {
    int reason = luaL_checkinteger(state, 1);
    int daughters = luaL_checkinteger(state, 2);
    bool wife = lua_toboolean(state, 3);
    g_Game->dungeon().AddCust(reason, daughters, wife);
    return 0;
}

int cLuaScript::AddFamilyToDungeon(lua_State *L) {
    int num_daughters = luaL_checkinteger(L, 1);
    bool mother = lua_toboolean(L, 2);

    // Set the surname for the family
    std::string surname = g_SurnameList.random();
    // `J` zzzzzz - this can probably be done easier
    std::shared_ptr<sGirl> Daughter1 = nullptr;
    std::shared_ptr<sGirl> Daughter2 = nullptr;
    std::shared_ptr<sGirl> Daughter3 = nullptr;
    std::shared_ptr<sGirl> Mother = nullptr;
    std::stringstream NGmsg1;
    std::stringstream NGmsg2;
    std::stringstream NGmsg3;
    std::stringstream NGmsgM;

    int oldest = 18;    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
    if (num_daughters > 0)
    {
        Daughter1 = g_Game->CreateRandomGirl(SpawnReason::KIDNAPPED, (g_Dice % 13) + 13);
        if (Daughter1->age() > oldest) oldest = Daughter1->age();
        Daughter1->SetSurname(surname);
    }
    if (num_daughters > 1)
    {
        Daughter2 = g_Game->CreateRandomGirl(SpawnReason::KIDNAPPED, (g_Dice % 13) + 13);
        if (Daughter2->age() == Daughter1->age())    // if only 2 daughters and their ages are the same, change that
        {                                            // if there is a third daughter, her age can be anything (to allow twins)
            if (Daughter1->age() > 20) Daughter2->age(-(g_Dice % 3 + 1));
            else Daughter2->age((g_Dice % 3 + 1));
        }
        if (Daughter2->age() > oldest) oldest = Daughter2->age();
        Daughter2->SetSurname(surname);
    }
    if (num_daughters > 2)
    {
        Daughter3 = g_Game->CreateRandomGirl(SpawnReason::KIDNAPPED, (g_Dice % 13) + 13);
        if (Daughter3->age() > oldest) oldest = Daughter3->age();
        Daughter3->SetSurname(surname);
    }

    if (mother)    // there is a mother
    {
        Mother = g_Game->CreateRandomGirl(SpawnReason::KIDNAPPED, (g_Dice % (50 - (oldest + 18))) + oldest + 18);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
        Mother->SetSurname(surname);
        if (!g_Dice.percent(Mother->age())) Mother->gain_trait("MILF");    // the younger the mother the more likely she will be a MILF
        Mother->lose_trait("Virgin");

        std::string biography = "Daughter of " + Mother->FullName() + " and a deadbeat brothel client.";

        if (Daughter1)    Daughter1->m_Desc = Daughter1->m_Desc + "\n \n" + biography;
        if (Daughter2)    Daughter2->m_Desc = Daughter2->m_Desc + "\n \n" + biography;
        if (Daughter3)    Daughter3->m_Desc = Daughter3->m_Desc + "\n \n" + biography;

        Mother->m_ChildrenCount[CHILD00_TOTAL_BIRTHS] += num_daughters;
        Mother->m_ChildrenCount[CHILD02_ALL_GIRLS] += num_daughters;
        Mother->m_ChildrenCount[CHILD04_CUSTOMER_GIRLS] += num_daughters;

    }
    std::string kstring = "kidnapped from her family";

    if (num_daughters > 0)
    {
        NGmsg1 << Daughter1->FullName() << " was " << kstring;
        if (num_daughters > 1 || mother)
        {
            NGmsg1 << " along with ";
            if (mother)                    NGmsg1 << "her mother " << Mother->FullName();
            if (num_daughters > 1 && mother)    NGmsg1 << " and ";
            if (num_daughters > 1)
            {
                NGmsg1 << "her sister" << (num_daughters > 2 ? "s " : "") << Daughter2->FullName();
                if (num_daughters > 2)            NGmsg1 << " and " << Daughter3->FullName();
            }
        }
        NGmsg1 << ".";
        Daughter1->AddMessage(NGmsg1.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
    }
    if (num_daughters > 1)
    {
        NGmsg2 << Daughter2->FullName() << " was " << kstring << " along with ";
        if (Mother) NGmsg2 << "her mother " << Mother->FullName() << " and ";
        NGmsg2 << "her sister" << (num_daughters > 2 ? "s " : " ") << Daughter1->FullName();
        if (num_daughters > 2) NGmsg2 << " and " << Daughter3->FullName();
        NGmsg2 << ".";
        Daughter2->AddMessage(NGmsg2.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
    }
    if (num_daughters > 2)
    {
        NGmsg3 << Daughter3->FullName() << " was " << kstring << " along with ";
        if (mother) NGmsg3 << "her mother " << Mother->FullName() << " and ";
        NGmsg3 << "her sisters " << Daughter1->FullName() << " and " << Daughter2->FullName() << ".";
        Daughter3->AddMessage(NGmsg3.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
    }
    if (mother)
    {
        NGmsgM << Mother->FullName() << " was " << kstring;
        if (num_daughters > 0)
        {
            NGmsgM << " along with her daughter" << (num_daughters > 2 ? "s " : " ") << Daughter1->FullName();
            if (num_daughters == 2)        NGmsgM << " and ";
            else if (num_daughters == 3) NGmsgM << ", ";
            if (num_daughters > 1)        NGmsgM << Daughter2->FullName();
            if (num_daughters > 2)        NGmsgM << " and " << Daughter3->FullName();
        }
        NGmsgM << ".";
        Mother->AddMessage(NGmsgM.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
    }

    if (Daughter1)    g_Game->dungeon().AddGirl(Daughter1, DUNGEON_GIRLKIDNAPPED);
    if (Daughter2)    g_Game->dungeon().AddGirl(Daughter2, DUNGEON_GIRLKIDNAPPED);
    if (Daughter3)    g_Game->dungeon().AddGirl(Daughter3, DUNGEON_GIRLKIDNAPPED);
    if (Mother)       g_Game->dungeon().AddGirl(Mother, DUNGEON_GIRLKIDNAPPED);

    return 0;
}

void cLuaScript::LoadSource(const std::string &file) {
    m_State.load(file);
}

void cLuaScript::PushParameter(sLuaParameter param)
{
    param.push(m_State);
}

bool cLuaScript::CheckFunction(const std::string& function) const {
    return m_State.has_function(function);
}

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
#include "CLog.h"
#include "cTariff.h"
#include "cGirls.h"
#include "buildings/cBrothel.h"
#include "Game.hpp"
#include "sConfig.h"
#include "character/predicates.h"

namespace settings {
    extern const char* MONEY_SELL_SLAVE;
    extern const char* MONEY_BUY_SLAVE;
    extern const char* GANG_WEAPON_UPGRADE_COST;
    extern const char* GANG_WAGES_FACTOR;
    extern const char* MONEY_COST_ROOM;
    extern const char* MONEY_COST_CONTRA;
    extern const char* MONEY_COST_HP;
    extern const char* MONEY_COST_NET;
}


extern cConfig cfg;

double cTariff::slave_base_price(sGirl& girl) const
{
    // The ask price is the base price for the girl. It changes with her stats, so we need to refresh it
    double cost;
    cGirls::UpdateAskPrice(girl, false);
    cost = girl.askprice() * 15;                        // base price is the girl's ask price stat
    for (int i = 0; i < NUM_SKILLS; i++)                // add to that the sum of her skills
    {
        cost += (unsigned int)girl.get_skill(i);
    }
    if (is_virgin(girl))    cost *= 1.5;    // virgins fetch a premium
    return cost;
}

int cTariff::slave_buy_price(sGirl& girl) const
{
    return int(slave_base_price(girl) * g_Game->settings().get_percent(settings::MONEY_BUY_SLAVE));
}

int cTariff::slave_sell_price(sGirl& girl) const
{

    double cost = slave_base_price(girl);
    return int(cost * g_Game->settings().get_percent(settings::MONEY_SELL_SLAVE));    // multiply by the config factor for selling slaves
}

int cTariff::empty_room_cost(IBuilding& brothel)
{

    double cost;
    /*
    *    basic cost is number of empty rooms
    *    nominal cost is 2 gold per
    *    modified by brothel support multiplier
    */
    cost = brothel.m_NumRooms - brothel.num_girls();
    cost *= 2;
    return int(cost);
}

int cTariff::goon_weapon_upgrade(int level)
{

    return int((level + 1) * g_Game->settings().get_integer(settings::GANG_WEAPON_UPGRADE_COST));
}

int cTariff::goon_mission_cost(int mission)
{

    double cost = 0.0;
    switch (mission)
    {
    case MISS_SABOTAGE:        cost = 150;    break;
    case MISS_SPYGIRLS:        cost = 40;        break;
    case MISS_CAPTUREGIRL:    cost = 125;    break;
    case MISS_EXTORTION:    cost = 116;    break;
    case MISS_PETYTHEFT:    cost = 110;    break;
    case MISS_GRANDTHEFT:    cost = 250;    break;
    case MISS_KIDNAPP:        cost = 150;    break;
    case MISS_CATACOMBS:    cost = 300;    break;
    case MISS_TRAINING:        cost = 90;        break;
    case MISS_RECRUIT:        cost = 80;        break;
    case MISS_SERVICE:        cost = 100;    break;
        //    case MISS_SAIGON:        just kidding
    case MISS_GUARDING:
    default:
        cost = 60;
        if (mission == MISS_GUARDING) break;    // `J` rearranged to make sure it works as intended
        g_LogFile.log(ELogLevel::ERROR, "cTariff: unrecognised goon mission ", mission, " changed to guard mission");
        break;
    }

    double factor = g_Game->settings().get_float(settings::GANG_WAGES_FACTOR);
    return int(cost * factor);
}




int cTariff::healing_price(int n)
{
    return int(n * g_Game->settings().get_integer(settings::MONEY_COST_HP));
}

int cTariff::nets_price(int n)
{
    return int(n * g_Game->settings().get_integer(settings::MONEY_COST_NET));
}

int cTariff::anti_preg_price(int n)
{
    return int(n * 2 * g_Game->settings().get_integer(settings::MONEY_COST_CONTRA));
}

int cTariff::advertising_costs(int budget)
{
    return int(budget);
}

int cTariff::add_room_cost(int n)
{
    return int(n * 1000 * g_Game->settings().get_integer(settings::MONEY_COST_ROOM));
}


double cTariff::slave_price(sGirl& girl, bool buying) const
{
    if (buying)
    {
        return slave_buy_price(girl);
    }
    return slave_sell_price(girl);
}


int cTariff::male_slave_sales()
{
    return g_Dice.random(300) + 200;
}

int cTariff::creature_sales()
{
    return g_Dice.random(2000) + 100;
}

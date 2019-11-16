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
}


extern cConfig cfg;

double cTariff::slave_base_price(sGirl *girl)
{
	// The ask price is the base price for the girl. It changes with her stats, so we need to refresh it
	double cost;
	cGirls::CalculateAskPrice(girl, false);
	cost = girl->askprice() * 15;						// base price is the girl's ask price stat
	for (u_int i = 0; i < NUM_SKILLS; i++)				// add to that the sum of her skills
	{
		cost += (unsigned int)girl->get_skill(i);
	}
	if (is_virgin(*girl))	cost *= 1.5;	// virgins fetch a premium
	return cost;
}

int cTariff::slave_buy_price(sGirl *girl)
{
	return int(slave_base_price(girl) * cfg.out_fact.slave_cost());
}

int cTariff::slave_sell_price(sGirl *girl)
{

	double cost = slave_base_price(girl);
	return int(cost * g_Game->settings().get_percent(settings::MONEY_SELL_SLAVE));	// multiply by the config factor for selling slaves
}

int cTariff::empty_room_cost(IBuilding& brothel)
{

	double cost;
	/*
	*	basic cost is number of empty rooms
	*	nominal cost is 2 gold per
	*	modified by brothel support multiplier
	*/
	cost = brothel.m_NumRooms - brothel.num_girls();
	cost *= 2;
	cost *= cfg.out_fact.brothel_support();
	return int(cost);
}

int cTariff::goon_weapon_upgrade(int level)
{

	return int((level + 1) * 1200 * cfg.out_fact.item_cost());
}

int cTariff::goon_mission_cost(int mission)
{

	double cost = 0.0, factor = cfg.out_fact.goon_wages();
	switch (mission)
	{
	case MISS_SABOTAGE:		cost = factor * 150;	break;
	case MISS_SPYGIRLS:		cost = factor * 40;		break;
	case MISS_CAPTUREGIRL:	cost = factor * 125;	break;
	case MISS_EXTORTION:	cost = factor * 116;	break;
	case MISS_PETYTHEFT:	cost = factor * 110;	break;
	case MISS_GRANDTHEFT:	cost = factor * 250;	break;
	case MISS_KIDNAPP:		cost = factor * 150;	break;
	case MISS_CATACOMBS:	cost = factor * 300;	break;
	case MISS_TRAINING:		cost = factor * 90;		break;
	case MISS_RECRUIT:		cost = factor * 80;		break;
	case MISS_SERVICE:		cost = factor * 100;	break;
		//	case MISS_SAIGON:		just kidding
	case MISS_GUARDING:
	default:
		cost = factor * 60;
		if (mission == MISS_GUARDING) break;	// `J` rearranged to make sure it works as intended
		g_LogFile.log(ELogLevel::ERROR, "cTariff: unrecognised goon mission ", mission, " changed to guard mission");
		break;
	}

	return int(cost);
}




int cTariff::healing_price(int n)
{
	return int(n * 10 * cfg.out_fact.consumables());
}

int cTariff::nets_price(int n)
{
	return int(n * 5 * cfg.out_fact.consumables());
}

int cTariff::anti_preg_price(int n)
{
	return int(n * 2 * cfg.out_fact.consumables());
}

/*
*	let's have matron wages go up as skill level increases.
*	`J` this is no longer used
*/
int cTariff::matron_wages(int level, int numgirls)
{
	int base = (level * 2) + numgirls * 2;
	return int(base * cfg.out_fact.matron_wages());
}

int cTariff::advertising_costs(int budget)
{
	return int(budget * cfg.out_fact.advertising());
}

int cTariff::add_room_cost(int n)
{
	return int(n * 1000 * cfg.out_fact.brothel_cost());
}


double cTariff::slave_price(sGirl *girl, bool buying)
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

int cTariff::girl_training()
{
	return int(cfg.out_fact.training() * 5);
}

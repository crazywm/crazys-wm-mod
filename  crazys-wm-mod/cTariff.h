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
#pragma once
#include "CLog.h"
#include "cRng.h"
#include "sConfig.h"

extern	CLog	g_LogFile;
extern	cRng	g_Dice;


struct sBrothel;
struct sGirl;

class cTariff {
	cConfig config;
	double slave_base_price(sGirl *girl);
public:
	cTariff() {}

	int goon_weapon_upgrade(int level);
	int goon_mission_cost(int mission);
	int healing_price(int n) {
		return int(n * 10 * config.out_fact.consumables());
	}
	int nets_price(int n) {
		return int(n * 5 * config.out_fact.consumables());
	}
	int anti_preg_price(int n) {
		return int(n * 2 * config.out_fact.consumables());
	}

	int strip_bar_price() {
		return int(2500 * config.out_fact.casino_cost());
	}
	int gambling_hall_price() {
		return int(15000 * config.out_fact.casino_cost());
	}

	int movie_cost() {
		return int(100 * config.out_fact.movie_cost());
	}
/*
 *	let's have matron wages go up as skill level increases.
 */
	int matron_wages(int level = 65) {
		int base = 200 + (level * 2);
		return int(base * config.out_fact.matron_wages());
	}
	int bar_staff_wages() {
		return int(20 * config.out_fact.bar_cost());
	}
	int empty_room_cost(sBrothel *brothel);
	int empty_bar_cost() {
		return int(20 * config.out_fact.bar_cost());
	}
	int active_bar_cost(int level, double shifts) {
		if(shifts > 2.0) shifts = 2.0;
		shifts /= 2.0;
		double cost = 50.0 * level / shifts;
		return int(cost * config.out_fact.bar_cost());
	}
	int empty_casino_cost(int level) {
		return int(50 * level * config.out_fact.casino_cost());
	}
	int active_casino_cost(int level, double shifts) {
		if(shifts > 2.0) shifts = 2.0;
		shifts /= 2.0;
		double cost = 150.0 * level / shifts;
		return int(cost * config.out_fact.casino_cost());
	}
	int casino_staff_wages() {
		g_LogFile.ss()
			<< "casino wages: config factor = "
			<< config.out_fact.staff_wages()
		;
		g_LogFile.ssend();
		return int(50 * config.out_fact.casino_cost());
	}
	int advertising_costs(int budget) {
		return int(budget * config.out_fact.advertising());
	}
	int add_room_cost(int n) {
		return int(n * 1000 * config.out_fact.brothel_cost());
	}

	double slave_price(sGirl *girl, bool buying) {
		if(buying) {
			return slave_buy_price(girl);
		}
		return slave_sell_price(girl);
	}
	int slave_buy_price(sGirl *girl);
	int slave_sell_price(sGirl *girl);

	int male_slave_sales() {
		return g_Dice.random(300) + 200;
	}
	int creature_sales() {
		return g_Dice.random(2000) + 100;
	}
	int girl_training() {
		return int(config.out_fact.training() * 5);
	}
/*
 *	really should do this by facility and match on name
 *
 *	that said...
 */
	int buy_facility(int base_price) {
		return int(config.out_fact.brothel_cost() * base_price);
	}
};

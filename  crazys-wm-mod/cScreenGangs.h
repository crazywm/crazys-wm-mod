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

#include "DirPath.h"
#include "cInterfaceWindow.h"
#include "InterfaceGlobals.h"

class cScreenGangs : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int back_id;			// Back button
	int gangcost_id;		// Gang Hiring Cost text
	int ganghire_id;		// Hire Gang button
	int gangfire_id;		// Fire Gang button
	int totalcost_id;		// Weekly Cost text
	int gold_id;			// Player Gold
	int ganglist_id;		// Controlled Gangs list
	int missionlist_id;		// Missions list
	int gangdesc_id;		// Gang Description text
	int missiondesc_id;		// Mission Description text
	int weaponlevel_id;		// Weapon Level text
	int weaponup_id;		// Increase Weapon Level button
	int netdesc_id;			// Nets Description text
	int netbuy_id;			// Buy 1 Net button
	int netbuy10_id;		// Buy 10 Nets button
	int netbuy20_id;		// Buy 20 Nets button
	int netautobuy_id;		// Auto-buy Nets checkbox
	int healdesc_id;		// Healing Potions Description text
	int healbuy_id;			// Buy 1 Healing Potion button
	int healbuy10_id;		// Buy 10 Healing Potions button
	int healbuy20_id;		// Buy 20 Healing Potions button
	int healautobuy_id;		// Auto-buy Healing Potions checkbox
	int recruitlist_id;		// Recruitable Gangs list

	int controlcatacombs_id;// checkbox
	int catacombslabel_id;	// just a label
	int ganggetsgirls_id;	// gang gets girls label
	int ganggetsitems_id;	// gang gets items label
	int ganggetsbeast_id;	// gang gets beast label
	int girlspercslider_id;	// girls percent slider
	int itemspercslider_id;	// items percent slider

	void set_ids();
	int		sel_pos;
	void hire_recruitable();
public:
	cScreenGangs();
	~cScreenGangs();

	void init();
	void process();
	void check_events();
	bool check_keys();

	int set_mission_desc(int mid);
	string mission_desc(int mid);
	string short_mission_desc(int mid);

	// XML screen replacement for cSelector, for multi-select listboxes
	int multi_first();
	int multi_next();
};

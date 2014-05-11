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
public:

private:
	char	buffer[256];
	int		sel_pos;

	static bool ids_set;
/*
 *	interface/event IDs
 */
	int back_id;		// Back button
	int gangcost_id;	// Gang Hiring Cost text
	int ganghire_id;	// Hire Gang button
	int gangfire_id;	// Fire Gang button
	int totalcost_id;	// Weekly Cost text
	int ganglist_id;	// Controlled Gangs list
	int missionlist_id;	// Missions list
	int gangdesc_id;	// Gang Description text
	int missiondesc_id;	// Mission Description text
	int weaponlevel_id;	// Weapon Level text
	int weaponup_id;	// Increase Weapon Level button
	int netdesc_id;		// Nets Description text
	int netbuy_id;		// Buy 20 Nets button
	int netautobuy_id;	// Auto-buy Nets checkbox
	int healdesc_id;	// Healing Potions Description text
	int healbuy_id;		// Buy 20 Healing Potions button
	int healautobuy_id;	// Auto-buy Healing Potions checkbox
	int recruitlist_id;	// Recruitable Gangs list

	void set_ids();

	void hire_recruitable();
public:
	cScreenGangs()
	{
		cConfig cfg;
		DirPath dp = DirPath()
			<< "Resources"
			<< "Interface"
			<< cfg.resolution.resolution()
			<< "gangs_screen.xml"
		;
		m_filename = dp.c_str();
	}
	~cScreenGangs() {}

	void init();
	void process();
	void check_events();
	bool check_keys();

	int set_mission_desc(int mid);
	string mission_desc(int mid);
	string short_mission_desc(int mid);

	// XML screen replacement for cSelector, for multi-select listboxes
	int multi_first() {
		sel_pos = 0;
		return GetNextSelectedItemFromList(ganglist_id, 0, sel_pos);
	}
	int multi_next() {
		return GetNextSelectedItemFromList(ganglist_id, sel_pos+1, sel_pos);
	}
};

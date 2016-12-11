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

class cScreenBuildingSetup : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int back_id;			// Back button
	int curbrothel_id;		// Current Brothel text
	int gold_id;			// Player Gold
	int potioncost_id;		// Potion Cost text
	int potionavail_id;		// Available Potions text
	int potions10_id;		// Buy 10 Potions button
	int potions20_id;		// Buy 20 Potions button
	int autopotions_id;		// Auto Buy Potions checkbox
	int barstaff_id;		// Bar Staff Cost text
	int barhire_id;			// Hire Bar Staff button
	int barfire_id;			// Fire Bar Staff button
	int casinostaff_id;		// Casino Staff Cost text
	int casinohire_id;		// Hire Casino Staff button
	int casinofire_id;		// Fire Casino Staff button
	int roomcost_id;		// Room Cost text
	int buyrooms_id;		// Add 5 Rooms button
	int restrict_id;		// Sex Restrictions text

	int noanal_id;			// Prohibit Anal checkbox
	int nobdsm_id;			// Prohibit BDSM checkbox
	int nobeast_id;			// Prohibit Bestiality checkbox
	int nofoot_id;			// Prohibit Hand Job checkbox
	int nogroup_id;			// Prohibit Group Sex checkbox
	int nohand_id;			// Prohibit Hand Job checkbox
	int nolesbian_id;		// Prohibit Lesbian checkbox
	int nonormal_id;		// Prohibit Normal Sex checkbox
	int nooral_id;			// Prohibit Oral checkbox
	int nostrip_id;			// Prohibit Hand Job checkbox
	int notitty_id;			// Prohibit Titty Sex checkbox

	int advertsli_id;		// Advertising Budget slider
	int advertamt_id;		// Advertising Budget value output text

	void set_ids();
public:
	cScreenBuildingSetup();
	~cScreenBuildingSetup();

	void init();
	void process();
	void check_events();
};

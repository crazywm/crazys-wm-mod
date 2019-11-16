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

#include "cGameWindow.h"

class cScreenBuildingSetup : public cGameWindow
{
private:
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

	int advertsli_id;		// Advertising Budget slider
	int advertamt_id;		// Advertising Budget value output text

	void set_ids() override;
	void set_sex_type_allowed(SKILLS sex_type, bool is_forbidden);

    struct STCB {
        int id;
        SKILLS skill;
    };

    std::vector<STCB> m_SexTypeAllowedMap;
public:
	cScreenBuildingSetup();

	void init(bool back) override;
	void process() override { };

    void buy_rooms();
    void buy_potions(int buypotions);
};

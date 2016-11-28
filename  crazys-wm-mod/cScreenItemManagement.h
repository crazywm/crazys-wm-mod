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

class cScreenItemManagement : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int curbrothel_id;	// Current Brothel text
	int back_id;		// Back button
	int sell10_l_id;	// Sell 10 Items Left button
	int sell10_r_id;	// Sell 10 Items Right button
	int sellall_l_id;	// Sell all Items Left button
	int sellall_r_id;	// Sell all Items Right button
	int buy10_l_id;		// Buy 10 Items Left button
	int buy10_r_id;		// Buy 10 Items Right button
	int shift_l_id;		// Shift Item Left button
	int shift_r_id;		// Shift Item Right button
	int gold_id;		// Player Gold text
	int desc_id;		// Item Description text
	int owners_l_id;	// Owners Left list
	int items_l_id;		// Items Left list
	int owners_r_id;	// Owners Right list
	int items_r_id;		// Items Right list
	int filter_id;		// Filter list
	int autouse_id;		// Autouse items checkbox
	int equip_l_id;		// Equip Left button
	int unequip_l_id;	// Unequip Left button
	int equip_r_id;		// Equip Right button
	int unequip_r_id;	// Unequip Right button
	int detail_l_id;	// Owner Details Left	`J` Added for .06.02.39
	int detail_r_id;	// Owner Details Right	`J` Added for .06.02.39

	int sel_pos_l;
	int sel_pos_r;
	enum Side {
		Left = 0,
		Right = 1
	};

	void set_ids();
public:
	cScreenItemManagement();
	~cScreenItemManagement();

	void init();
	void process();
	void check_events();
	bool check_keys();
	void check_buttons();

	void attempt_transfer(Side transfer_from, int num = 1);
	void refresh_item_list(Side which_list);
	string GiveItemText(int goodbad, int HateLove, sGirl* targetgirl, string ItemName = "");
	sGirl* GirlSelectedFromList(int owner, int selectedfromlist = -100);

	void write_item_text(sInventoryItem * item, int owner, int target);

	// XML screen replacement for cSelector, for multi-select listboxes
	int multi_left_first() {
		sel_pos_l = 0;
		return GetNextSelectedItemFromList(items_l_id, 0, sel_pos_l);
	}
	int multi_left_next() {
		return GetNextSelectedItemFromList(items_l_id, sel_pos_l + 1, sel_pos_l);
	}
	int multi_right_first() {
		sel_pos_r = 0;
		return GetNextSelectedItemFromList(items_r_id, 0, sel_pos_r);
	}
	int multi_right_next() {
		return GetNextSelectedItemFromList(items_r_id, sel_pos_r + 1, sel_pos_r);
	}
};

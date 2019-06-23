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

class cScreenSlaveMarket : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int back_id;			// Back button
	int more_id;			// More Details button
	int buy_slave_id;		// Buy button
	int cur_brothel_id;		// Current Brothel text
	int slave_list_id;		// Slave listbox
	int trait_list_id;		// Trait listbox
	int trait_list_text_id;	// Trait list text
	int details_id;			// Girl Details text
	int girl_desc_id;		// Girl desc text
	int trait_id;			// Trait text
	int image_id;			// Image of selected slave
	int header_id;			// page header text ("Slave Market")
	int slave_market_id;	// "Slave Market" text
	int gold_id;			// Player's Gold

	int releaseto_id;		// Where to release girls to Text
	int roomsfree_id;		// how many rooms are available 
	int house_id;			// Player's House button
	int clinic_id;			// Clinic's House button
	int studio_id;			// Studio's House button
	int arena_id;			// Arena's House button
	int centre_id;			// Centre's House button
	int farm_id;			// Farm's button
	int dungeon_id;			// Dungeon's button
	int brothel0_id;		// Brothel 0 button
	int brothel1_id;		// Brothel 1 button
	int brothel2_id;		// Brothel 2 button
	int brothel3_id;		// Brothel 3 button
	int brothel4_id;		// Brothel 4 button
	int brothel5_id;		// Brothel 5 button
	int brothel6_id;		// Brothel 6 button

	int		selection;
	int		DetailLevel;
	int		ImageNum;
	int		sel_pos;
	void set_ids();
public:
	cScreenSlaveMarket();
	~cScreenSlaveMarket();

	void init();
	void process();

	bool buy_slaves();			// `J` used when buying slaves
	void change_release(string towhere);
	void preparescreenitems(sGirl* girl);

	void generate_unique_girl(int i, bool &unique);
	bool check_events();
	bool check_keys();
	int &image_num() { return ImageNum; }
	int &selected_item() { return selection; }
	bool change_selected_girl();
	string get_buy_slave_string(sGirl* girl);

	int multi_slave_first() {
		sel_pos = 0;
		return GetNextSelectedItemFromList(slave_list_id, 0, sel_pos);
	}

	int multi_slave_next() {
		return GetNextSelectedItemFromList(slave_list_id, sel_pos + 1, sel_pos);
	}
};

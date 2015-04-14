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

class cBuilding;

class cScreenSlaveMarket : public cInterfaceWindowXML
{
public:

private:
	int		selection;
	int		DetailLevel;
	int		ImageNum;
	char	buffer[256];
	int		sel_pos;

	static bool ids_set;
/*
 *	interface/event IDs
 */
	int back_id;		// Back button
	int more_id;		// More Details button
	int buy_slave_id;	// Buy button
	int cur_brothel_id;	// Current Brothel text
	int slave_list_id;	// Slave listbox
	int trait_list_id;	// Trait listbox
	int details_id;		// Girl Details text
	int trait_id;		// Trait text
	int slave_image_id;	// Image of selected slave
	int header_id;		// page header text ("Slave Market")

	void set_ids();
public:
	cScreenSlaveMarket()
	{
		
		DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "slavemarket_screen.xml";
		m_filename = dp.c_str();
		selection = -1;
		ImageNum = -1;
		DetailLevel = 0;
		sel_pos = 0;
	}
	~cScreenSlaveMarket() {}

	void init();
	void process();

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
		return GetNextSelectedItemFromList(slave_list_id, sel_pos+1, sel_pos);
	}
};
